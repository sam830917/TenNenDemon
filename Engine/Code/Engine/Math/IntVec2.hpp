#pragma once

struct IntVec2
{
public:
	int x = 0;
	int y = 0;

	static const IntVec2 ZERO;
	static const IntVec2 ONE;

public:
	// Construction/Destruction
	~IntVec2() = default;
	IntVec2() = default;
	IntVec2( const IntVec2& copyFrom );
	explicit IntVec2( int initialX, int initialY );

	// Accessors (const methods)
	float            GetLength() const;
	int              GetLengthSquared() const;
	int              GetTaxicabLength() const;
	float            GetOrientationRadians() const;
	float            GetOrientationDegrees() const;
	const IntVec2    GetRotated90Degrees() const;
	const IntVec2    GetRotatedMinus90Degrees() const;

	// Mutators (non-const methods)
	void            Rotate90Degrees();
	void            RotateMinus90Degrees();
	void			SetFromText( const char* text );

	// Operators (self-mutating / non-const)
	bool            operator==( const IntVec2& compare ) const;
	bool            operator!=( const IntVec2& compare ) const;
	const IntVec2		operator+( const IntVec2& vecToAdd ) const;		// IntVec2 + IntVec2
	const IntVec2		operator-( const IntVec2& vecToSubtract ) const;	// IntVec2 - IntVec2
	const IntVec2		operator-() const;								// -IntVec2, i.e. "unary negation"
	const IntVec2		operator*( int uniformScale ) const;			// IntVec2 * int
	const IntVec2		operator*( const IntVec2& vecToMultiply ) const;	// IntVec2 * IntVec2
	const IntVec2		operator/( float inverseScale ) const;			// IntVec2 / float

	// Operators (self-mutating / non-const)
	void			operator+=( const IntVec2& vecToAdd );				// IntVec2 += IntVec2
	void			operator-=( const IntVec2& vecToSubtract );		// IntVec2 -= IntVec2
	void			operator*=( const int uniformScale );			// IntVec2 *= int
	void			operator/=( const int uniformDivisor );		// IntVec2 /= int
	void			operator=( const IntVec2& copyFrom );				// IntVec2 = IntVec2

	// Standalone "friend" functions that are conceptually, but not actually, part of IntVec2::
	friend const IntVec2 operator*( int uniformScale, const IntVec2& vecToScale );	// int * IntVec2
};