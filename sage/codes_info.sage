import sys

load codes.py

len = len(sys.argv)
p = 2
l = 4
lamb = 2

if ("--help" in sys.argv) or ("-h" in sys.argv):
    print("This program will gather and print out all needed info about Ms and Rads structure.")
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

output_name = "codes_info_{p}-{l}-{lamb}.txt".format( \
        p = p, l = l, lamb = lamb)

output_file = open(output_name, mode='w')
sys.stdout = output_file

p    = int(p)
l    = int(l)
lamb = int(lamb)

if (l % lamb):
    raise ValueError("lamb must be a factor of l")

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


print("Info for graph with parameters: " \
        + "p = {0}, l = {1}, lambda = {2}.".format(p, l ,lamb))
print("q = {0}, pi = {1}, m = {2}". format(q, pi, m))
print("\n")

print("Total number of (M_pi)-s: {0}".format( numofMs))
print("Total number of (M_p)-s:  {0}".format(nilindex))
print("Total number of different non-zero radical powers: {0}".format(nilindex - 1))
print("\n")

print("m*(pi-1) = {0:.<10} m*(pi-1) - 1 = {1:.<10}".format( numofMs - 1,  numofMs - 2))
print("l*(p-1) =  {0:.<10} l*(p-1) - 1 =  {1:.<10}".format(nilindex - 1, nilindex - 2))

print("\n")
print("Total number of Ms is {0}:".format(numofMs))
for i in range(0, numofMs):
    print("M_{pi}({m},{k}):\t\tdim = {dim}". format(pi = pi, m = m, k = i, dim = m_k(i, pi, m)))

print("\n")
print("Total number of Radical powers is {0}:".format(nilindex))
for i in range(0, nilindex):
    print( "Rad^{s} = M_{p}({l},{j}):\tdim = {dim}". format(s = l*(p - 1) - i, \
            p = p, l = l, j = i, dim = m_k(i, p, l)))

Ms = [M_k(0)]
for i in range(1, numofMs):
    Ms.append(M_k(i, Ms[i-1]))

RMs = [ gap.ProductSpace(Rad, M) for M in Ms ]


print("\n")
print("Detected (M_pi)-s <-> Rads equalities:")
for j in range(1, nilindex):
    try:
        index = Ms.index(Rads[j])
        print ("Rad^{s} == M_{pi}({m},{k})".format(s = j, pi = pi, m = m, k = index))
    except:
        pass

print("\n")
print("Detected Rad*M-s <-> M-s equalities:")
for j in range(0, numofMs):
    try:
        index = Ms.index(RMs[j])
        print ("Rad*M_{pi}({m},{j}) == M_{pi}({m},{k})".format(j = j, pi = pi, m = m, k = index))
    except:
        pass
