#include "Utils.h"

bool RectContains(const Rectangle &r, int x, int y)
{
	return r.Location.X <= x && r.Location.Y <= y && r.Location.X + r.Size.X > x && r.Location.Y + r.Size.Y > y;
}

bool RectContains(const Rectangle &r, Int2 p)
{
	return r.Location.X <= p.X && r.Location.Y <= p.Y && r.Location.X + r.Size.X > p.X && r.Location.Y + r.Size.Y > p.Y;
}

bool RectContains(const Rectangle &r, float x, float y)
{
	return r.Location.X <= x && r.Location.Y <= y && r.Location.X + r.Size.X > x && r.Location.Y + r.Size.Y > y;
}

bool RectContains(const Rectangle &r, Float2 p)
{
	return r.Location.X <= p.X && r.Location.Y <= p.Y && r.Location.X + r.Size.X > p.X && r.Location.Y + r.Size.Y > p.Y;
}

bool RectContains(const Rectangle &r, const Rectangle &r2)
{
	return r.Location.X <= r2.Location.X && r.Location.Y <= r2.Location.Y && r.Location.X + r.Size.X >= r2.Location.X + r2.Size.X && r.Location.Y + r.Size.Y >= r2.Location.Y + r2.Size.Y;
}

float AddBigValues(float a, float b)
{
    // Should be safe:
    if ((a > 0) != (b > 0))
        return a + b;
    if (a < 0 && b < 0)
        return -AddBigValues(-a, -b);

    if (MAX_float - a < b)
        return MAX_float;

    return a + b;
}

float AddBigValues(float a, float b, float c)
{
    return AddBigValues(AddBigValues(a, b), c);
}

float AddBigValues(float a, float b, float c, float d)
{
    return AddBigValues(AddBigValues(AddBigValues(a, b), c), d);
}

Float2 AddBigValues(Float2 a, Float2 b)
{
    // Should be safe:
    return Float2(AddBigValues(a.X, b.X), AddBigValues(a.Y, b.Y));
}

Float2 AddBigValues(Float2 a, Float2 b, Float2 c)
{
    // Should be safe:
    return AddBigValues(AddBigValues(a, b), c);
}

Float2 AddBigValues(Float2 a, Float2 b, Float2 c, Float2 d)
{
    // Should be safe:
    return AddBigValues(AddBigValues(AddBigValues(a, b), c), d);
}


int AddBigValues(int a, int b)
{
    // Should be safe:
    if ((a > 0) != (b > 0))
        return a + b;
    if (a < 0 && b < 0)
        return -AddBigValues(-a, -b);

    if (MAX_int32 - a < b)
        return MAX_int32;

    return a + b;
}

int AddBigValues(int a, int b, int c)
{
    return AddBigValues(AddBigValues(a, b), c);
}

int AddBigValues(int a, int b, int c, int d)
{
    return AddBigValues(AddBigValues(AddBigValues(a, b), c), d);
}

Int2 AddBigValues(Int2 a, Int2 b)
{
    // Should be safe:
    return Int2(AddBigValues(a.X, b.X), AddBigValues(a.Y, b.Y));
}

Int2 AddBigValues(Int2 a, Int2 b, Int2 c)
{
    // Should be safe:
    return AddBigValues(AddBigValues(a, b), c);
}

Int2 AddBigValues(Int2 a, Int2 b, Int2 c, Int2 d)
{
    // Should be safe:
    return AddBigValues(AddBigValues(AddBigValues(a, b), c), d);
}

