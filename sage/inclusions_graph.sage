import sys
import matplotlib.pyplot as plt

load codes.py

len = len(sys.argv)
p = 2
l = 4
lamb = 2
imgsize = 10

if ("--help" in sys.argv) or ("-h" in sys.argv):
    print("This program will contruct and output inclusion tree.")
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

output_name = "inclusion_tree_{p}-{l}-{lamb}.png".format( \
        p = p, l = l, lamb = lamb)

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

print("\nComputing M-s...")
Ms = [M_k(0, pi, m)]
for i in range(1, numofMs):
    Ms.append(M_k(i, pi, m, Ms[i-1]))

print("\nGenerating elements mapping...")

def gen_elems():
    elems = {Ms[i]: " "*20 + "M_{pi}({m},{i})".format(pi = pi, m = m, i = i) for i in range(0, numofMs)}

    for j in range(1, nilindex + 1):
        try:
            i = Ms.index(Rads[j])
            elems.update( {Ms[i]: " "*40 + "M_{pi}({m},{i})".format(pi = pi, m = m, i = i) \
                + " = " + "Rad^{0}".format(j)} )
        except:
            elems.update( {Rads[j]: " "*20 + "Rad^{j}".format(j = j)} )
    return elems

elems = gen_elems()

print("\nConstructing graph...")
def construct_IG():
    IG = DiGraph()

    for e in elems.keys():
        IG.add_vertex(e)


    for e in IG.vertices():
        for g in IG.vertices():
            if cmp_gap(e, g):
                IG.add_edge(e, g)

    return IG

def replace_gap(v):
    return elems.values()[elems.keys().index(v)]

def relabel_IG(IG):
    IG.relabel(replace_gap, inplace = True)

def draw_IG(IG, img):
    img.clf()
    graph = IG.transitive_reduction().plot(vertex_size = 50)
    graph.save(output_name, figure = img)

IG = construct_IG()
# this is where MAGIC happens! Without this reconstruction SAGE fails to produce good output.
IG = construct_IG()

img = plt.figure(figsize = (imgsize,imgsize))

relabel_IG(IG)
draw_IG(IG, img)

print("\nDone.\n")

def help_message():
    print("Please view the produced output now.")
    print("If everything looks good, press `q` to exit.")
    print("If graph looks bad, press `r` and we will redraw it for you.")
    print("If graph looks borked (missing or extra vertices, incorrect labels, etc.), press `g` and we will regenerate it for you.")
    print("If graph is too small press `b` to make it bigger and if graph is too big press `s` to make it smaller.")

help_message()
c = raw_input()
while (c != "q"):
    if (c == "r"):
        draw_IG(IG, img)
        print("Redraw completed.")
        c = raw_input()
    elif (c == "b"):
        img.clf()
        imgsize += 5
        img = plt.figure(figsize = (imgsize,imgsize))
        draw_IG(IG, img)
        print("Redraw completed.")
        c = raw_input()
    elif (c == "s"):
        img.clf()
        imgsize -= 5
        if (imgsize <= 0):
            print("WARNING! You are trying to minify image size lower than zero limit.")
            print("Size is forcedly set to 5.")
            imgsize = 5
        img = plt.figure(figsize = (imgsize,imgsize))
        draw_IG(IG, img)
        print("Redraw completed.")
        c = raw_input()
    elif (c == "g"):
        elems = gen_elems()

        IG = construct_IG()
        relabel_IG(IG)
        draw_IG(IG, img)
        print("Regeneration completed.")
        c = raw_input()
    else:
        help_message()
        c = raw_input()
