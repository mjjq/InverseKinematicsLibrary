#ifndef MATH_H
#define MATH_H

namespace Math
{
    const float PI = 3.14159265359f;

    float fastInvSqrt(float number);

    int dot(sf::Vector2i const & vec1,
            sf::Vector2i const & vec2);

    float dot(sf::Vector2f const & vec1,
              sf::Vector2f const & vec2);

    float dot(sf::Vector3f const & v1,
              sf::Vector3f const & v2);

    float square(sf::Vector2f const & vec);

    float square(sf::Vector3f const & vec);

    sf::Vector2f norm(sf::Vector2i const & vec1);

    sf::Vector2f norm(sf::Vector2f const & vec1);

    sf::Vector2f fastNorm(sf::Vector2f const & vec1);

    sf::Vector3f norm(sf::Vector3f const & vec);

    float cross(sf::Vector2f const & v1,
                sf::Vector2f const & v2);

    sf::Vector3f cross(sf::Vector3f const & v1,
                       sf::Vector3f const & v2);

    sf::Vector2f orthogonal(sf::Vector2f const & v,
                            float const & scalar);

    sf::Vector2f rotate(sf::Vector2f const & vec1,
                        float const & angleDegree);

    sf::Vector2f tripleProduct(sf::Vector2f const & v1,
                               sf::Vector2f const & v2,
                               sf::Vector2f const & v3);

    sf::Vector2f average(std::vector<sf::Vertex > const & verts);

    float average(std::vector<float> const & vec);

    void printVector(sf::Vector2f const & vec);

    void printVector(sf::Vector2i const & vec);

    int modulo(int const & n1, int const & n2);
}

#endif // SF_VECTOR_MATH_H
