#ifndef PHOTON_MAPPING_CAMERA_HPP
#define PHOTON_MAPPING_CAMERA_HPP

#include "mathFunctions.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"
#include "Ray.hpp"
#include "Extent.hpp"

namespace photon_mapping {
    class Camera {
        private:
            float m_fieldOfView;
            Matrix43f m_transformation;
            Matrix43f m_inverseTransformation;

        public:
            Camera()
                : m_fieldOfView(degToRad(75.0f))
                , m_transformation(Matrix43f::IDENTITY)
                , m_inverseTransformation(Matrix43f::IDENTITY)
            {}

            float fieldOfView() const {
                return m_fieldOfView;
            }
            void setFieldOfView(float fieldOfView) {
                m_fieldOfView = fieldOfView;
            }

            Matrix43f transformation() const {
                return m_transformation;     
            }
            void setTransformation(Matrix43f const& transformation) {
                m_transformation = transformation;  
                m_inverseTransformation = ~transformation;
            }

            Matrix43f inverseTransformation() const {
                return m_inverseTransformation;     
            }
            void setInverseTransformation(Matrix43f const& inverseTransformation) { 
                m_inverseTransformation = ~inverseTransformation;
                m_transformation = ~m_inverseTransformation;
            }

            Ray cast(Point2f const& surfacePosition, Extent2i const& surfaceExtent) const {
                Vector2f imageHalfDiagonal = diagonal(surfaceExtent) / 2.0f;
                Vector2f pixelHalfDiagonal = Vector2f(0.5f, 0.5f);
                float imageDistance = surfaceExtent.y / (2.0f * tan(m_fieldOfView / 2.0f));
                Vector3f pixelDirection = unit(Point3f(surfacePosition - imageHalfDiagonal + pixelHalfDiagonal, -imageDistance) - Point3f::ORIGIN);
                return Ray(Point3f::ORIGIN, pixelDirection) * m_transformation;
            }

            Point3f position() const {
                return Point3f(m_transformation.s30, m_transformation.s31, m_transformation.s32);
            }
           
            Vector3f left() const {
                return -right();
            }

            Vector3f right() const {
                return Vector3f(m_inverseTransformation.s00, m_inverseTransformation.s10, m_inverseTransformation.s20);
            }

            Vector3f up() const {
                return Vector3f(m_inverseTransformation.s01, m_inverseTransformation.s11, m_inverseTransformation.s21);
            }

            Vector3f down() const {
                return -up(); 
            }

            Vector3f forward() const {
                return -back();
            }

            Vector3f back() const {
                return Vector3f(m_inverseTransformation.s02, m_inverseTransformation.s12, m_inverseTransformation.s22);
            }
    };
}

#endif