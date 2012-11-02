import sys

load codes.py

len = len(sys.argv)
p = 2
l = 4
lamb = 2

if ("--help" in sys.argv) or ("-h" in sys.argv):
    print("This program will run some tests of codes.py module.")
    print("\nThis program requires at least 3 arguments:")
    print("--p x    -- base prime integer")
    print("--l y    -- exponent specifying field and group size")
    print("--lamb z -- multiple of l specifying pi")
    sys.exit()

if len < 4:
    sys.exit("You must specify at least p, l and lamb. See --help.")
else:
    if "--p" in sys.argv:
        pos = sys.argv.index("--p")
        p = sys.argv[pos + 1]
        sys.argv.remove("--p")
        sys.argv.remove(p)
    else:
        sys.exit("You must specify at least p, l and lamb. See --help.")

    if "--l" in sys.argv:
        pos = sys.argv.index("--l")
        l = sys.argv[pos + 1]
        sys.argv.remove("--l")
        sys.argv.remove(l)
    else:
        sys.exit("You must specify at least p, l and lamb. See --help.")

    if "--lamb" in sys.argv:
        pos = sys.argv.index("--lamb")
        lamb = sys.argv[pos + 1]
        sys.argv.remove("--lamb")
        sys.argv.remove(lamb)
    else:
        sys.exit("You must specify at least p, l and lamb. See --help.")

p    = int(p)
l    = int(l)
lamb = int(lamb)

if (l % lamb):
    raise ValueError("lamb must be factor of l")

q = p ** l
pi = p ** lamb
m = l // lamb

nilindex = l*(p-1) + 1
numofMs = m*(pi-1) + 1

Q  = gap.GF(p, l)
H  = gap.AbelianGroup([p]*l)

S = gap.GroupRing(Q, H)
phi = gap.Embedding(H, S)
psi = gap.Embedding(Q, S)

Rad  = gap.AugmentationIdeal(S)
Rads = gap.AugmentationIdealPowerSeries(S)

Hbasis = H.MinimalGeneratingSet().AsSortedList()
Qbasis = Q.CanonicalBasis().AsSortedList()
Sbasis = S.CanonicalBasis().AsSortedList()
V = VectorSpace(Zmod(p), l)

print("Theorem 3.1")
print("This theorem states that R_s(q) == S and R_s(q-1) == Rad")
th_31="(R_s(q) == S) and (R_s(q-1) == Rad)"
print(th_31)
print(eval(th_31))

print("\n")
print("Lemma 3.2")
print("This lemma consists of three statements:")
print("a) u_s(i)*u_s(j) == 0 if (0 <= i + j <= q - 2)")
print("b) u_s(i)*u_s(j) == -((-1)^(t - delta))*binomial(t, delta)*u_s(delta) == ")
print("     == -((-1)^(s - delta))*binomial(s, delta)*u_s(delta) if \
i + j == q - 1 + delta < 2*(q - 1)")
print("c) u_s(q-1)*u_s(q-1) == 2*u_s(q-1) - e")
print("a): ret = True")
print("for t in range(0,q):")
print("    for s in range(0,q):")
print("        if (s + t) <= (q-2):")
print("            ret &= (u_s(s)*u_s(t) == S.Zero())")
ret = True
for t in range(0,q):
    for s in range(0,q):
        if (s + t) <= (q - 2):
            ret &= (u_s(s)*u_s(t) == S.Zero())
print(ret)
print("b): ret = True")
print("for t in range(0,q):")
print("    for s in range(0,q):")
print("        if ((s+t) > (q-2)) and ((s+t) < 2*(q-1)):")
print("            delta = s + t - (q - 1)")
print("            coeff_t = -((-1)^(t - delta)) * binomial(t, delta)")
print("            ret &= (u_s(s)*u_s(t) == coeff_t*u_s(delta))")
ret = True
for t in range(0,q):
    for s in range(0,q):
        if ((s+t) > (q-2)) and ((s+t) < 2*(q-1)):
            delta = s + t - (q - 1)
            coeff_t = -((-1)^(t - delta)) * binomial(t, delta)
            ret &= (u_s(s)*u_s(t) == coeff_t*u_s(delta))
print(ret)
print("c): u = u_s(q-1)")
print("u*u == (2*u - S.One())")
u = u_s(q-1)
ret = (u*u == (2*u - S.One()))
print(ret)

print("\nComputing M-s...")
Ms = [M_k(0)]
for i in range(1, numofMs):
    Ms.append(M_k(i, Ms[i-1]))

print("Useful info.")
print("Total number of M-s is {0}.".format(numofMs))
print("Total number of Radical powers is {0}.".format(nilindex - 1))

print("\n")
print("Theorem 5.5 (Asmuss, Key)")
print("This theorem states that dimension of M_pi(m, k) equals to m_pi(m, k),")
print("where m_pi(m, k) is defined as sum of ((-1)**j)*binomial(m, j)*binomial(m + k - pi*j, k - pi*j)")
print("for j from 0 to m.")
print("ret = True")
print("for i in range(0, numofMs):")
print("    ret &= (gap.Dimension(Ms[i]) == m_k(i, pi, m))")
ret = True
for i in range(0, numofMs):
    ret &= (gap.Dimension(Ms[i]) == m_k(i, pi, m))
print(ret)

print("\n")
print("Theorem 5.2")
print("This theorem states that Ann(M_pi(m, k)) == M_pi(m, m*(pi - 1) - 1 - k) \
for 0 <= k <= m*(pi - 1) - 1")
print("ret = True")
print("for k in range(0, m*(pi - 1)):")
print("    ret &= (gap.Dimension(gap.ProductSpace(Ms[k], Ms[m*(pi - 1) - 1 - k])) == 0)")
ret = True
for k in range(0, m*(pi - 1)):
    ret &= (gap.Dimension(gap.ProductSpace(Ms[k], Ms[m*(pi - 1) - 1 - k])) == 0)
print(ret)

print("\n")
print("Proposition 2.1")
print("This proposition states that dumension of Rad^j equals to p^l - m_k(j - 1, p, l)")
print("ret = True")
print("for i in range(nilindex, 0, -1):")
print("    ret &= (gap.Dimension(Rads[i]) == q - m_k(i - 1, p, l))")
ret = True
for i in range(nilindex, 0, -1):
    ret &= (gap.Dimension(Rads[i]) == q - m_k(i - 1, p, l))
print(ret)


print("\nM-s with their dimensions:")
for i in range(0, numofMs):
    print("M_{pi}({m},{k}): dim = {dim}".format(pi = pi, m = m, k = i, dim = m_k(i, pi, m)))

print("\nRadical powers with their dimensions:")
for i in range(nilindex - 1, 0,-1):
    print("Rad^{j}: dim = {dim}". format(j = i, dim = q - m_k(i - 1, p, l) ))
