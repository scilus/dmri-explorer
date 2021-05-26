#version 460

void computeAssocLegendre(float x)
{
    const int lmax = 8;
    plm[0] = 1.0f; // identity for p00
    for(int mm = 1; mm <= lmax; ++mm) // fill up p11, p22, ..., p88
    {
        plm[j(mm, mm)] = (1.0f - 2.0f * mm) * plm[j(mm - 1, mm - 1)] * sqrt(1.0f - x*x);
    }
    for(int mm = 0; mm < lmax; ++mm) // fill up p10, p21, ..., p87
    {
        plm[j(mm+1, mm)] = (2.0f * mm + 1) * plm[j(mm, mm)] * x;
    }
    for(int mm = 0; mm < lmax; ++mm) // remaining positive plm
    {
        for(int ll = 2 + mm; ll <= lmax; ++ll)
        {
            plm[j(ll, mm)] = ((2.0f * ll - 1) * x * plm[j(ll-1, mm)] - (ll + mm - 1.0f) * plm[j(ll - 2, mm)]) / float(ll - mm);
        }
    }
    // Fill up negative degrees
    for(int ll = 1; ll <= lmax; ++ll)
    {
        for(int mm = -ll; mm < 0; ++mm)
        {
            plm[j(ll, mm)] = pow(-1.0, mm) * factorial(ll - mm) / factorial(ll + mm) * plm[j(ll, abs(mm))];
        }
    }
}

float assoc_legendre(int l, int m)
{
    return plm[j(l, m)];
}

float sph_harmonic_complex_radius(int l, int m, float theta)
{
    float r = (2.0f * l + 1) / 4.0f / PI
            * factorial(l - m) / factorial(l + m);

    r = sqrt(r) * assoc_legendre(l, m);
    return r;
}

float sph_harmonic_r(int l, int m, float theta, float phi)
{
    float r = sph_harmonic_complex_radius(l, m, theta);
    return r * cos(m * phi);
}

float sph_harmonic_i(int l, int m, float theta, float phi)
{
    float r = sph_harmonic_complex_radius(l, m, theta);
    return r * sin(m * phi);
}

float real_sh_descoteaux(int l, int m, float theta, float phi)
{
    if(m < 0)
    {
        return sqrt(2) * sph_harmonic_r(l, abs(m), theta, phi);
    }
    else if(m > 0)
    {
        return sqrt(2) * sph_harmonic_i(l, m, theta, phi);
    }
    else
    {
        return sph_harmonic_r(l, m, theta, phi);
    }
}