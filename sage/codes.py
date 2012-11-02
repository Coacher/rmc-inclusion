#!/usr/bin/env python2

# import everything
from sage import *

# this package comes with a needed AugmentationIdeal defintion
gap('LoadPackage("laguna")')

def base_representation(x, w = 1, base = 2):
    '''Generates (base**w)-adic representation.'''

    b = base ** w
    div = x // b
    digits = [x % b]

    while (div != 0):
        digits = [div % b] + digits
        div //=  b
    return digits

def weight(x, w = 1, base = 2):
    '''Calculates (base**w)-adic weight.'''

    return sum(base_representation(x, w, base))

# SAGE's prod expects list, has to redefine
def my_prod(x,y):
    return x*y

def my_pow(x,y):
    return x**int(y)

def zipWith(aa, bb, f):
    '''Zip two lists in one using supplied function.'''

    ret = []
    for a,b in zip(aa, bb):
        ret += [ f(a,b) ]
    return ret

def v_to_F(v, F, Fbasis):
    '''Return F element with Fbasis coordinates equal to v.'''

    v = zipWith(v, Fbasis, my_prod)

    ret = F.Zero()
    for x in v:
        ret += x
    return ret

def v_to_G(v, G, Gbasis):
    '''Return G element with Gbasis coordinates equal to v.'''

    v = zipWith(Gbasis, v, my_pow)

    ret = G.One()
    for x in v:
        ret *= x
    return ret

def u_s(s):
    ret = S.Zero()
    for v in V:
        h = gap.Image(phi, v_to_G(v, H, Hbasis))
        a = v_to_F(v, Q, Qbasis) ** s
        ret += a*h
    return ret

def R_s(s, prev = None):
    if prev:
        Ret = prev
        Ret += gap.Ideal(S, [u_s(s - 1)])
    else:
        Ret  = gap.Ideal(S, [u_s(0)])
        Ret += gap.Ideal(S, [u_s(i) for i in range(1,s)])
    return Ret

def M_k(k, prev = None):
    '''Return M_pi(m,k).

    You should use prev paramater to support M_(k-1)
    parameter for M_k which WILL speed up process.
    '''
    if prev:
        Ret = prev
        for i in range(0,q):
            if (weight(i, base = pi) == k):
                Ret += gap.Ideal(S, [u_s(i)])
    else:
        Ret = gap.Ideal(S, [u_s(0)])
        for i in range(0,q):
            if (weight(i, base = pi) <= k):
                Ret += gap.Ideal(S, [u_s(i)])
    return Ret

def m_k(k, pi, m):
    return sum([ ( (-1)**j) * binomial(m, j) * binomial(m + k - pi*j, k - pi*j ) for j in range(0, m + 1) ])

def cmp_gap(x, y):
    if (gap.IsSubspace(x, y)):
        return True
    if (gap.EQ(x, y)):
        return True
    return False
