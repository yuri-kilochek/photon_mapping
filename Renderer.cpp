#include "Renderer.hpp"

#include <QDebug>

#include "mathFunctions.hpp"

namespace photon_mapping {
    Photon Renderer::emitAndScatter(Random &random) {
        float currentRefractionIndex = 1.0;
        Scene::Sample sample;
        do {
            sample = scene()->uniformOnSurface(random);
        } while (luminance(sample.emission) * sample.emissionPower == 0.0f);
        float hitDistance; Scene::Sample hit;
        Ray ray;
        ray.origin = sample.position;
        do {
            ray.direction = random.cosineDirection(sample.normal);
        } while (!scene()->hit(ray, hitDistance, hit));
        Photon photon;
        photon.position = hit.position;
        photon.direction = ray.direction;
        photon.power = sample.emission * sample.emissionPower;
        while (true) {
            float action = random.uniformInRange01();

            float specularLuminance = luminance(hit.specular);
            if (action < specularLuminance) {
                Ray reflectedRay;
                reflectedRay.origin = hit.position;
                reflectedRay.direction = reflect(ray.direction, hit.normal);
                reflectedRay.origin += reflectedRay.direction * 0.001f;
                ray = reflectedRay;
                if (!scene()->hit(ray, hitDistance, hit)) {
                    photon.power = Color::BLACK;
                    break;
                }
                photon.position = hit.position;
                photon.direction = ray.direction;
                continue;
            } else {
                action -= specularLuminance;
            }

            float transmissionLuminance = luminance(hit.transmission);
            if (action < transmissionLuminance) {
                Ray transmittedRay;
                transmittedRay.origin = hit.position;
                if (dot(hit.normal, ray.direction) <= 0.0f) {
                    transmittedRay.direction = transmit(ray.direction, +hit.normal, 1.0f / hit.refractionIndex);
                } else {
                    transmittedRay.direction = transmit(ray.direction, -hit.normal, hit.refractionIndex / 1.0f);
                }
                transmittedRay.origin += transmittedRay.direction * 0.001f;
                ray = transmittedRay;
                if (nan(sqrLength(ray.direction)) || !scene()->hit(ray, hitDistance, hit)) {
                    photon.power = Color::BLACK;
                    break;
                }
                photon.position = hit.position;
                photon.direction = ray.direction;
                continue;
            } else {
                action -= transmissionLuminance;
            }

            // absorbtion;
            break;
        }
        return photon;
    }

    void Renderer::scatterPart(Random& random, std::vector<Photon>::iterator begin, std::vector<Photon>::iterator end) {
        if (scene()->empty()) {
            return;
        }
        for (std::vector<Photon>::iterator p = begin; p != end; ++p) {
            *p = emitAndScatter(random);
        }
    }

    Color Renderer::render(std::vector<PhotonMap::NearestPhoton>& nearestPhotons, Ray const& ray, int rayBounceCount) const {      
        Color radiance = Color::BLACK;
        
        float hitDistance; Scene::Sample hit;
        if (!scene()->hit(ray, hitDistance, hit)) {
            return radiance;
        }
        
        { // emitted radiance
            radiance += hit.emission * hit.emissionPower;
        }
        { // photon radiance
            Color photonRadiance = Color::BLACK; 
            auto nearestPhotonsEnd = photonMap().nearest(hit.position, nearestPhotons.begin(), nearestPhotons.end());
            Vector3f const& n = hit.normal;
            Vector3f v = -ray.direction;
            auto farthestPhotonIt = nearestPhotons.begin();
            float farthestPhotonSqrDistance = farthestPhotonIt->sqrDistance;
            for (auto& nearestPhoton : nearestPhotons) {
                Photon const& photon = *nearestPhoton.photon;
                Vector3f l = -photon.direction;
                float ln = dot(l, n);
                if (ln > 0.0f) {
                    photonRadiance += hit.diffuse * ln * photon.power;
                    Vector3f r = reflect(-l, n);
                    float rv = dot(r, v);
                    if (rv > 0.0f) {
                        photonRadiance += hit.specular * pow(rv, hit.specularPower) * photon.power;
                    }
                }
            }
            photonRadiance /= pi() * farthestPhotonSqrDistance * photonCount();
            radiance += photonRadiance;
        }
        { // reflected radiance
            if (rayBounceCount + 1 < maxRayBounceCount() && luminance(hit.specular) > 0.0f) {
                Ray reflectedRay;
                reflectedRay.origin = hit.position;
                reflectedRay.direction = reflect(ray.direction, hit.normal);
                reflectedRay.origin += reflectedRay.direction * 0.001f;
                radiance += hit.specular * render(nearestPhotons, reflectedRay, rayBounceCount + 1);
            }
        }
        { // transmitted radiance
            if (rayBounceCount + 1 < maxRayBounceCount() && luminance(hit.transmission) > 0.0f) {
                Ray transmittedRay;
                transmittedRay.origin = hit.position;
                if (dot(hit.normal, ray.direction) <= 0.0f) {
                    transmittedRay.direction = transmit(ray.direction, +hit.normal, 1.0f / hit.refractionIndex);
                } else {
                    transmittedRay.direction = transmit(ray.direction, -hit.normal, hit.refractionIndex / 1.0f);
                }
                transmittedRay.origin += transmittedRay.direction * 0.001f;
                if (!nan(sqrLength(transmittedRay.direction))) {
                    radiance += hit.transmission * render(nearestPhotons, transmittedRay, rayBounceCount + 1);
                }
            }
        }
        return radiance;
    }

    void Renderer::renderPart(std::vector<PhotonMap::NearestPhoton>& nearestPhotons, Segment2i const& segment) {
        Extent2i sufraceExtent = surface()->extent();
        if (quality() < 0) {
            float idealSampleSize = 1 << -quality();
            Extent2i pixelCount = extent(segment);
            idealSampleSize = min(idealSampleSize, (float)pixelCount.x);
            idealSampleSize = min(idealSampleSize, (float)pixelCount.y);
            Extent2i sampleCount;
            sampleCount.x = pixelCount.x / idealSampleSize;
            sampleCount.y = pixelCount.y / idealSampleSize;
            Extent2i pixelCountPerSample;
            pixelCountPerSample.x = pixelCount.x / sampleCount.x;
            pixelCountPerSample.y = pixelCount.y / sampleCount.y;
            Extent2i leftoverPixelCount;
            leftoverPixelCount.x = pixelCount.x - pixelCountPerSample.x * sampleCount.x;
            leftoverPixelCount.y = pixelCount.y - pixelCountPerSample.y * sampleCount.y;
            Point2i pixelPositionBegin;
            Point2i pixelPositionEnd;
            Point2f samplePosition;
            Point2i sample;
            for (sample.y = 0; sample.y < sampleCount.y; ++sample.y) {
                pixelPositionBegin.y = segment.min.y + pixelCountPerSample.y * (sample.y + 0) + min(sample.y + 0, leftoverPixelCount.y);
                pixelPositionEnd.y = segment.min.y + pixelCountPerSample.y * (sample.y + 1) + min(sample.y + 1, leftoverPixelCount.y);
                samplePosition.y = floor((pixelPositionBegin.y + pixelPositionEnd.y) / 2.0f);
                for (sample.x = 0; sample.x < sampleCount.x; ++sample.x) {
                    pixelPositionBegin.x = segment.min.x + pixelCountPerSample.x * (sample.x + 0) + min(sample.x + 0, leftoverPixelCount.x);
                    pixelPositionEnd.x = segment.min.x + pixelCountPerSample.x * (sample.x + 1) + min(sample.x + 1, leftoverPixelCount.x);
                    samplePosition.x = floor((pixelPositionBegin.x + pixelPositionEnd.x) / 2.0f);
                    Ray ray = camera()->cast(samplePosition, sufraceExtent);
                    Color sampleColor = render(nearestPhotons, ray);
                    Color pixelColor = clamp(Color::BLACK, sampleColor, Color::WHITE);
                    Point2i pixelPosition;
                    for (pixelPosition.y = pixelPositionBegin.y; pixelPosition.y < pixelPositionEnd.y; ++pixelPosition.y) {
                        for (pixelPosition.x = pixelPositionBegin.x; pixelPosition.x < pixelPositionEnd.x; ++pixelPosition.x) {
                            checkInterrupts();
                            surface()->setPixel(pixelPosition, pixelColor);
                        }
                    }
                }
            }
        } else {
            int sampleCount = 1 << quality();
            float sampleDistance = 1.0f / sampleCount;
            float sampleWeight = 1.0f / sqr(sampleCount);
            Point2i pixelPosition;
            for (pixelPosition.y = segment.min.y; pixelPosition.y < segment.max.y; ++pixelPosition.y) {
                for (pixelPosition.x = segment.min.x; pixelPosition.x < segment.max.x; ++pixelPosition.x) {
                    checkInterrupts();
                    Color sampleColor = Color::BLACK;
                    Point2f subPixelPosition;
                    for (subPixelPosition.y = sampleDistance / 2.0f; subPixelPosition.y < 1.0f; subPixelPosition.y += sampleDistance) {
                        for (subPixelPosition.x = sampleDistance / 2.0f; subPixelPosition.x < 1.0f; subPixelPosition.x += sampleDistance) {
                            Point2f samplePosition = Point2f(pixelPosition.x + subPixelPosition.x, pixelPosition.y + subPixelPosition.y);
                            Ray ray = camera()->cast(samplePosition, sufraceExtent);
                            sampleColor += render(nearestPhotons, ray) * sampleWeight;
                        }
                    }
                    Color pixelColor = clamp(Color::BLACK, sampleColor, Color::WHITE);
                    surface()->setPixel(pixelPosition, pixelColor);
                }
            }
        }
    }
}