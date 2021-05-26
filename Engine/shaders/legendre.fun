#version 460

float p00(float x)
{
    return 1.0f;
}

float p10(float x)
{
    return x;
}

float p11(float x)
{
    return - sqrt(1.0f - x*x);
}

float p1m1(float x)
{
    return -0.5f * p11(x);
}

float p20(float x)
{
    return 0.5f * (3.0f * x*x - 1.0f);
}

float p21(float x)
{
    return -3.0f * x * sqrt(1.0f - x*x);
}

float p22(float x)
{
    return 3.0f * (1.0f - x*x);
}

float p2m1(float x)
{
    return -1.0f / 6.0f * p21(x);
}

float p2m2(float x)
{
    return 1.0f / 24.0f * p22(x);
}

float p30(float x)
{
    return 0.5f * (5.0f * x*x*x - 3.0f * x);
}

float p31(float x)
{
    return - 3.0f / 2.0f * (5.0f*x*x - 1.0f) * pow((1.0f - x*x), 0.5f);
}

float p32(float x)
{
    return 15.0f * x * (1.0f - x*x);
}

float p33(float x)
{
    return -15.0f * pow(1.0f - x*x, 3.0f/2.0f);
}

float p3m1(float x)
{
    return -1.0f / 12.0f * p31(x);
}

float p3m2(float x)
{
    return 1.0f / 120.0f * p32(x);
}

float p3m3(float x)
{
    return -1.0f / 720.0f * p33(x);
}

float p40(float x)
{
    return 1.0f / 8.0f * (35.0f * pow(x, 4) - 30.0f * x*x + 3);
}

float p41(float x)
{
    return -5.0f / 2.0f * (7 * pow(x, 3) - 3.0f * x) * sqrt(1.0f - x*x);
}

float p42(float x)
{
    return 15.0f / 2.0f * (7.0f * x*x - 1.0f) * (1.0f - x*x);
}

float p43(float x)
{
    return -105.0f * x * pow(1.0f - x*x, 1.5f);
}

float p44(float x)
{
    return 105.0f * pow(1.0f - x*x, 2.0f);
}

float p4m1(float x)
{
    return -1.0f / 20.0f * p41(x);
}

float p4m2(float x)
{
    return 1.0f / 360.0f * p42(x);
}

float p4m3(float x)
{
    return -1.0f / 5040.0f * p43(x);
}

float p4m4(float x)
{
    return 1.0f / 40320.0f * p44(x);
}

float assocLegendreApprox(int l, int m, float x)
{
    if(l > 2)
    {
        return 0.0f;
    }
    switch(l)
    {
    case 0:
        return p00(x);
    case 1:
        if(m == -1)
        {
            return p1m1(x);
        }
        else if(m == 0)
        {
            return p10(x);
        }
        else if(m == 1)
        {
            return p11(x);
        }
        break;
    case 2:
        if(m == -2)
        {
            return p2m2(x);
        }
        else if(m == -1)
        {
            return p2m1(x);
        }
        else if(m == 0)
        {
            return p20(x);
        }
        else if(m == 1)
        {
            return p21(x);
        }
        else if(m == 2)
        {
            return p22(x);
        }
        break;
    case 3:
        if(m == -3)
        {
            return p3m3(x);
        }
        else if(m == -2)
        {
            return p3m2(x);
        }
        else if(m == -1)
        {
            return p3m1(x);
        }
        else if(m == 0)
        {
            return p30(x);
        }
        else if(m == 1)
        {
            return p31(x);
        }
        else if(m == 2)
        {
            return p32(x);
        }
        else if(m == 3)
        {
            return p33(x);
        }
        break;
    case 4:
        if(m == -4)
        {
            return p4m4(x);
        }
        else if(m == -3)
        {
            return p4m3(x);
        }
        else if(m == -2)
        {
            return p4m2(x);
        }
        else if(m == -1)
        {
            return p4m1(x);
        }
        else if(m == 0)
        {
            return p40(x);
        }
        else if(m == 1)
        {
            return p41(x);
        }
        else if(m == 2)
        {
            return p42(x);
        }
        else if(m == 3)
        {
            return p43(x);
        }
        else if(m == 4)
        {
            return p44(x);
        }
        break;
    }
    return 0.0f;
}
