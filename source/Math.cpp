/**
    @file sfVectorMath.cpp
    Purpose: Various mathematical vector operation functions for SFML's
    sf::Vector types. Includes dot product, norm etc.

    @author mjjq
*/

#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "Math.h"

const float PI{3.14159265359f};

float Math::fastInvSqrt(float number)
{
    const float x2 = number * 0.5F;
	const float threehalfs = 1.5F;

	union {
		float f;
		uint32_t i;
	} conv = {number}; // member 'f' set to value of 'number'.
	conv.i  = 0x5f3759df - ( conv.i >> 1 );
	conv.f  *= ( threehalfs - ( x2 * conv.f * conv.f ) );
	return conv.f;
}

/**
    Get the dot product between two 2D integer vectors.

    @param vec1 The first vector.
    @param vec2 The second vector.

    @return Dot product of the two vectors.
*/
int Math::dot(sf::Vector2i const & vec1, sf::Vector2i const & vec2)
{
    int prod = vec1.x*vec2.x + vec1.y*vec2.y;
    return prod;
}


/**
    Get the dot product between two 2D float vectors.

    @param vec1 The first vector.
    @param vec2 The second vector.

    @return Dot product of the two vectors.
*/
float Math::dot(sf::Vector2f const & vec1, sf::Vector2f const & vec2)
{
    float prod = vec1.x*vec2.x + vec1.y*vec2.y;
    return prod;
}


float Math::dot(sf::Vector3f const & v1, sf::Vector3f const & v2)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}


float Math::square(sf::Vector2f const & vec)
{
    float square = vec.x*vec.x + vec.y*vec.y;
    return square;
}


float Math::square(sf::Vector3f const & vec)
{
    return vec.x*vec.x + vec.y*vec.y + vec.z*vec.z;
}


/**
    Get the unit/normlised vector of an integer vector.

    @param vec1 The vector to normalise.

    @return The unit/normalised vector.
*/
sf::Vector2f Math::norm(sf::Vector2i const & vec1)
{
    sf::Vector2f normVec = static_cast<sf::Vector2f>(vec1)/static_cast<float>(pow(dot(vec1,vec1),0.5));
    return normVec;
}


/**
    Get the unit/normlised vector of a float vector.

    @param vec1 The vector to normalise.

    @return The unit/normalised vector.
*/
sf::Vector2f Math::norm(sf::Vector2f const & vec1)
{
    return vec1/sqrtf(square(vec1));
}

sf::Vector2f Math::fastNorm(sf::Vector2f const & vec1)
{
    return vec1*fastInvSqrt(vec1.x*vec1.x + vec1.y*vec1.y);
}


sf::Vector3f Math::norm(sf::Vector3f const & vec)
{
    return vec / sqrtf(square(vec));
}


float Math::cross(sf::Vector2f const & v1, sf::Vector2f const & v2)
{
    return v1.x*v2.y - v1.y*v2.x;
}


sf::Vector3f Math::cross(sf::Vector3f const & v1,
                                 sf::Vector3f const & v2)
{
    float a = v1.y * v2.z - v1.z * v2.y;
    float b = v1.z * v2.x - v1.x * v2.z;
    float c = v1.x * v2.y - v1.y * v2.x;

    return sf::Vector3f(a,b,c);
}


sf::Vector2f Math::orthogonal(sf::Vector2f const & v, float const & scalar)
{
    return {v.y*scalar, -v.x*scalar};
}


/**
    Rotate the vector clockwise by a chosen angle

    @param vec1 The vector to rotate.
    @param angleDegree The angle to rotate by in degrees.

    @return The rotated vector.
*/
sf::Vector2f Math::rotate(sf::Vector2f const & vec1, float const & angleDegree)
{
    float cosine = cosf(PI * angleDegree / 180.0f);
    float sine = sinf(PI * angleDegree / 180.0f);

    float vec1xprime = cosine*vec1.x - sine*vec1.y;
    float vec1yprime = sine*vec1.x + cosine*vec1.y;

    return sf::Vector2f{vec1xprime,vec1yprime};
}


sf::Vector2f Math::tripleProduct(sf::Vector2f const & v1,
                                         sf::Vector2f const & v2,
                                         sf::Vector2f const & v3)
{
    sf::Vector2f result = v2 * Math::dot(v3, v1) -
                        v1 * Math::dot(v2, v3);
    return result;
}


sf::Vector2f Math::average(std::vector<sf::Vertex > const & verts)
{
    sf::Vector2f average = {0.0f, 0.0f};
    if(verts.size() > 0)
    {
        for(int i=0; i<(int)verts.size(); ++i)
            average += verts[i].position;
        average = average/(float)verts.size();
    }

    return average;
}


float Math::average(std::vector<float> const & vec)
{
    float average = 0.0f;
    for(auto &element : vec)
        average += element;
    average = average/vec.size();

    return average;
}


/**
    Print the float vector, format (x, y).

    @param vec The vector to print.

    @return Void.
*/
void Math::printVector(sf::Vector2f const & vec)
{
   std::cout << "(" << vec.x << ", " << vec.y << ")\n";
}


/**
    Print the integer vector, format (x, y).

    @param vec The vector to print.

    @return Void.
*/
void Math::printVector(sf::Vector2i const & vec)
{
    std::cout << "(" << vec.x << ", " << vec.y << ")\n";
}


int Math::modulo(int const & n1, int const & n2)
{
    int m = n1 % n2;
    return m + (m < 0 ? n2 : 0);
}
