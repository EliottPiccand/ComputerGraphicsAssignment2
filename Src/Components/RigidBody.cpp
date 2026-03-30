#include "Components/RigidBody.h"

#include "Components/Cannonball.h"
#include "Events/EventQueue.h"
#include "Events/Explosion.h"
#include "Events/RemoveGameObject.h"
#include "GameObject.h"
#include "Utils/Constants.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <numbers>
#include <print>
#include <unordered_map>
#include <vector>

using namespace component;

namespace
{

constexpr size_t PHYSICS_SOLVER_ITERATIONS = 4;
constexpr float POSITION_CORRECTION_RATIO = 0.35f;
constexpr float POSITION_EPSILON = 1.0e-4f;
constexpr float MISSILE_COLLISION_EXPLOSION_RADIUS = 50.0f;

struct CollisionInfo
{
    bool collides = false;
    glm::vec2 normal = {0.0f, 1.0f};
    float penetration = 0.0f;
};

float cross2(const glm::vec2 &a, const glm::vec2 &b)
{
    return a.x * b.y - a.y * b.x;
}

glm::vec2 cross2(float s, const glm::vec2 &v)
{
    return glm::vec2(-s * v.y, s * v.x);
}

glm::vec2 polygonCentroid(const ConvexPolygon &polygon)
{
    if (polygon.empty())
    {
        return {0.0f, 0.0f};
    }

    glm::vec2 center(0.0f);
    for (const auto &p : polygon)
    {
        center += p;
    }

    return center / static_cast<float>(polygon.size());
}

std::vector<glm::vec2> toWorldPolygon(const ConvexPolygon &polygon, const glm::mat3 &transform)
{
    std::vector<glm::vec2> world;
    world.reserve(polygon.size());

    for (const auto &p : polygon)
    {
        const glm::vec3 worldPoint = transform * glm::vec3(p, 1.0f);
        world.push_back(glm::vec2(worldPoint));
    }

    return world;
}

bool projectAlongAxis(const std::vector<glm::vec2> &polygon, const glm::vec2 &axis, float &minProj, float &maxProj)
{
    if (polygon.empty())
    {
        return false;
    }

    minProj = std::numeric_limits<float>::infinity();
    maxProj = -std::numeric_limits<float>::infinity();

    for (const auto &point : polygon)
    {
        const float d = glm::dot(point, axis);
        minProj = std::min(minProj, d);
        maxProj = std::max(maxProj, d);
    }

    return true;
}

bool satHelper(const std::vector<glm::vec2> &a, const std::vector<glm::vec2> &b, CollisionInfo &result)
{
    for (size_t i = 0; i < a.size(); ++i)
    {
        const glm::vec2 current = a[i];
        const glm::vec2 next = a[(i + 1) % a.size()];
        const glm::vec2 edge = next - current;

        if (glm::dot(edge, edge) < POSITION_EPSILON)
        {
            continue;
        }

        const glm::vec2 axis = glm::normalize(glm::vec2(-edge.y, edge.x));

        float minA = 0.0f;
        float maxA = 0.0f;
        float minB = 0.0f;
        float maxB = 0.0f;
        projectAlongAxis(a, axis, minA, maxA);
        projectAlongAxis(b, axis, minB, maxB);

        const float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap <= 0.0f)
        {
            return false;
        }

        if (overlap < result.penetration)
        {
            result.penetration = overlap;
            result.normal = axis;
            result.collides = true;
        }
    }

    return true;
}

CollisionInfo satCollision(const std::vector<glm::vec2> &a, const std::vector<glm::vec2> &b)
{
    CollisionInfo result;
    result.penetration = std::numeric_limits<float>::infinity();

    if (a.size() < 3 || b.size() < 3)
    {
        return result;
    }

    if (!satHelper(a, b, result))
    {
        return CollisionInfo{};
    }

    if (!satHelper(b, a, result))
    {
        return CollisionInfo{};
    }

    const glm::vec2 centerA = polygonCentroid(a);
    const glm::vec2 centerB = polygonCentroid(b);
    if (glm::dot(centerB - centerA, result.normal) < 0.0f)
    {
        result.normal = -result.normal;
    }

    return result;
}

glm::vec2 findContactPoint(const std::vector<glm::vec2> &incident, const std::vector<glm::vec2> &reference, const glm::vec2 &normal)
{
    float bestIncident = std::numeric_limits<float>::infinity();
    glm::vec2 contactIncident = incident.front();
    for (const auto &point : incident)
    {
        const float d = glm::dot(point, normal);
        if (d < bestIncident)
        {
            bestIncident = d;
            contactIncident = point;
        }
    }

    float bestReference = -std::numeric_limits<float>::infinity();
    glm::vec2 contactReference = reference.front();
    for (const auto &point : reference)
    {
        const float d = glm::dot(point, normal);
        if (d > bestReference)
        {
            bestReference = d;
            contactReference = point;
        }
    }

    return 0.5f * (contactIncident + contactReference);
}

float estimateInertia(const ConvexPolygon &polygon, float mass)
{
    if (polygon.empty())
    {
        return 1.0f;
    }

    const glm::vec2 center = polygonCentroid(polygon);
    float sum = 0.0f;
    for (const auto &point : polygon)
    {
        const glm::vec2 d = point - center;
        sum += glm::dot(d, d);
    }

    const float avgRadius2 = sum / static_cast<float>(polygon.size());
    return std::max(mass * avgRadius2, 1.0e-3f);
}

void clampBodyInsideWorld(const ConvexPolygon &collider, const std::shared_ptr<Transform> &transform)
{
    const glm::mat3 model = transform->resolve();
    const auto worldPoly = toWorldPolygon(collider, model);
    if (worldPoly.empty())
    {
        return;
    }

    float minX = std::numeric_limits<float>::infinity();
    float minY = std::numeric_limits<float>::infinity();
    float maxX = -std::numeric_limits<float>::infinity();
    float maxY = -std::numeric_limits<float>::infinity();

    for (const auto &p : worldPoly)
    {
        minX = std::min(minX, p.x);
        minY = std::min(minY, p.y);
        maxX = std::max(maxX, p.x);
        maxY = std::max(maxY, p.y);
    }

    glm::vec2 correction(0.0f);
    if (minX < 0.0f)
    {
        correction.x = -minX;
    }
    else if (maxX > WORLD_WIDTH)
    {
        correction.x = WORLD_WIDTH - maxX;
    }

    if (minY < 0.0f)
    {
        correction.y = -minY;
    }
    else if (maxY > WORLD_HEIGHT)
    {
        correction.y = WORLD_HEIGHT - maxY;
    }

    if (glm::dot(correction, correction) > 0.0f)
    {
        transform->translate(correction);
    }
}

} // namespace

RigidBody::RigidBody(ConvexPolygon collider, bool fixed, float mass, float restitution)
    : collider(std::move(collider)), mass(std::max(mass, 1.0e-3f)), restitution(std::clamp(restitution, 0.0f, 1.0f)), fixed(fixed)
{
    if (this->collider.empty())
    {
        this->collider = {
            {-0.5f, -0.5f},
            {0.5f, -0.5f},
            {0.5f, 0.5f},
            {-0.5f, 0.5f},
        };
    }

    if (fixed)
    {
        invMass = 0.0f;
        invInertia = 0.0f;
    }
    else
    {
        invMass = 1.0f / this->mass;
        inertia = estimateInertia(this->collider, this->mass);
        invInertia = 1.0f / inertia;
    }
}

RigidBody::~RigidBody()
{
    const auto it = std::find_if(bodies.begin(), bodies.end(), [&](auto el) { return el.lock().get() == this;});
    if (it != bodies.end())
    {
        bodies.erase(it);
        std::println("rb destroyed");
    }
}

void RigidBody::initialize()
{
    bodies.push_back(std::dynamic_pointer_cast<RigidBody>(shared_from_this()));

    const auto transformOption = owner.lock()->findFirstComponentInParents<Transform>();
    assert(transformOption.has_value() &&
           "No transform found! component::RigidBody needs its node or one of its parents has a component::Transform");
    transform = transformOption.value();

    const auto model = transform.lock()->resolve();
    previousState.position = glm::vec2(model[2]);
    previousState.rotation = transform.lock()->getRotation();
    previousState.velocity = {0.0f, 0.0f};
    previousState.angular_velocity = 0.0f;
}

void RigidBody::update(float deltaTime)
{
    (void)deltaTime;
}

void RigidBody::simulateAll(float deltaTime)
{
    if (deltaTime <= 0.0f)
    {
        return;
    }

    std::vector<std::shared_ptr<RigidBody>> liveBodies;
    liveBodies.reserve(bodies.size());
    std::unordered_map<GameObjectId, glm::vec2> cannonballsToExplode;

    for (auto it = bodies.begin(); it != bodies.end();)
    {
        if (auto body = it->lock())
        {
            liveBodies.push_back(body);
            ++it;
        }
        else
        {
            it = bodies.erase(it);
        }
    }

    for (auto &body : liveBodies)
    {
        auto transform = body->transform.lock();
        auto owner = body->owner.lock();
        const bool isCannonball = owner && owner->getComponent<Cannonball>().has_value();

        if (!transform)
        {
            continue;
        }

        const glm::mat3 model = transform->resolve();
        const glm::vec2 position = glm::vec2(model[2]);
        const float rotation = transform->getRotation();

        body->velocity = (position - body->previousState.position) / deltaTime;

        float rotationDelta = rotation - body->previousState.rotation;
        if (rotationDelta > std::numbers::pi_v<float>)
        {
            rotationDelta -= 2.0f * std::numbers::pi_v<float>;
        }
        else if (rotationDelta < -std::numbers::pi_v<float>)
        {
            rotationDelta += 2.0f * std::numbers::pi_v<float>;
        }

        body->angularVelocity = isCannonball ? 0.0f : (rotationDelta / deltaTime);
    }

    for (size_t iter = 0; iter < PHYSICS_SOLVER_ITERATIONS; ++iter)
    {
        for (size_t i = 0; i < liveBodies.size(); ++i)
        {
            auto &a = liveBodies[i];
            auto ta = a->transform.lock();
            if (!ta)
            {
                continue;
            }

            const glm::mat3 modelA = ta->resolve();
            const auto worldA = toWorldPolygon(a->collider, modelA);
            const glm::vec2 centerA = glm::vec2(modelA[2]);

            for (size_t j = i + 1; j < liveBodies.size(); ++j)
            {
                auto &b = liveBodies[j];
                auto tb = b->transform.lock();
                if (!tb)
                {
                    continue;
                }

                const glm::mat3 modelB = tb->resolve();
                const auto worldB = toWorldPolygon(b->collider, modelB);
                const glm::vec2 centerB = glm::vec2(modelB[2]);

                CollisionInfo collision = satCollision(worldA, worldB);
                if (!collision.collides || collision.penetration <= POSITION_EPSILON)
                {
                    continue;
                }

                const auto ownerA = a->owner.lock();
                const auto ownerB = b->owner.lock();
                const auto aCannonball = ownerA ? ownerA->getComponent<Cannonball>() : std::nullopt;
                const auto bCannonball = ownerB ? ownerB->getComponent<Cannonball>() : std::nullopt;
                const bool aIsCannonball = aCannonball.has_value();
                const bool bIsCannonball = bCannonball.has_value();

                const bool ignoreShooterCollision =
                    (aIsCannonball && ownerB && aCannonball.value()->getShooterId() == ownerB->getId()) ||
                    (bIsCannonball && ownerA && bCannonball.value()->getShooterId() == ownerA->getId());

                if (ignoreShooterCollision)
                {
                    continue;
                }

                if (aIsCannonball)
                {
                    cannonballsToExplode.try_emplace(ownerA->getId(), centerA);
                }

                if (bIsCannonball)
                {
                    cannonballsToExplode.try_emplace(ownerB->getId(), centerB);
                }

                // Cannonballs are kinematic (driven by Cannonball::update). They still trigger
                // collision events, but are excluded from impulse-based rigid-body resolution.
                if (aIsCannonball || bIsCannonball)
                {
                    continue;
                }

                const float invMassSum = a->invMass + b->invMass;
                if (invMassSum <= POSITION_EPSILON)
                {
                    continue;
                }

                const float positionalCorrection = collision.penetration * POSITION_CORRECTION_RATIO;
                const glm::vec2 correctionA = -collision.normal * positionalCorrection * (a->invMass / invMassSum);
                const glm::vec2 correctionB = collision.normal * positionalCorrection * (b->invMass / invMassSum);

                if (!a->fixed)
                {
                    ta->translate(correctionA);
                }

                if (!b->fixed)
                {
                    tb->translate(correctionB);
                }

                const glm::vec2 contact = findContactPoint(worldB, worldA, collision.normal);
                const glm::vec2 ra = contact - centerA;
                const glm::vec2 rb = contact - centerB;

                const glm::vec2 va = a->velocity + cross2(a->angularVelocity, ra);
                const glm::vec2 vb = b->velocity + cross2(b->angularVelocity, rb);
                const glm::vec2 relativeVelocity = vb - va;
                const float velocityAlongNormal = glm::dot(relativeVelocity, collision.normal);
                if (velocityAlongNormal > 0.0f)
                {
                    continue;
                }

                const float raCrossN = cross2(ra, collision.normal);
                const float rbCrossN = cross2(rb, collision.normal);
                const float denom = invMassSum + (raCrossN * raCrossN) * a->invInertia + (rbCrossN * rbCrossN) * b->invInertia;
                if (denom <= POSITION_EPSILON)
                {
                    continue;
                }

                const float e = std::min(a->restitution, b->restitution);
                const float impulseMag = -(1.0f + e) * velocityAlongNormal / denom;
                const glm::vec2 impulse = impulseMag * collision.normal;

                if (!a->fixed)
                {
                    a->velocity -= impulse * a->invMass;
                    a->angularVelocity -= cross2(ra, impulse) * a->invInertia;
                }

                if (!b->fixed)
                {
                    b->velocity += impulse * b->invMass;
                    b->angularVelocity += cross2(rb, impulse) * b->invInertia;
                }
            }
        }
    }

    constexpr float LINEAR_DAMPING = 0.98f;
    constexpr float ANGULAR_DAMPING = 0.98f;

    for (auto &body : liveBodies)
    {
        auto transform = body->transform.lock();
        auto owner = body->owner.lock();
        const bool isCannonball = owner && owner->getComponent<Cannonball>().has_value();

        if (!transform || body->fixed)
        {
            continue;
        }

        if (isCannonball)
        {
            const glm::mat3 model = transform->resolve();
            body->previousState.position = glm::vec2(model[2]);
            body->previousState.rotation = transform->getRotation();
            body->previousState.velocity = body->velocity;
            body->previousState.angular_velocity = 0.0f;
            body->angularVelocity = 0.0f;
            continue;
        }

        transform->translate(body->velocity * deltaTime);
        transform->rotate(body->angularVelocity * deltaTime);

        clampBodyInsideWorld(body->collider, transform);

        body->velocity *= LINEAR_DAMPING;
        body->angularVelocity *= ANGULAR_DAMPING;

        const glm::mat3 model = transform->resolve();
        body->previousState.position = glm::vec2(model[2]);
        body->previousState.rotation = transform->getRotation();
        body->previousState.velocity = body->velocity;
        body->previousState.angular_velocity = body->angularVelocity;
    }

    for (const auto &[cannonballId, collisionPosition] : cannonballsToExplode)
    {
        EventQueue::post<event::RemoveGameObject>(cannonballId);
        EventQueue::post<event::Explosion>(collisionPosition, MISSILE_COLLISION_EXPLOSION_RADIUS);
    }
}
