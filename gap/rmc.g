# Packages used: Alnuth, AutPGrp, GUAVA, LAGUNA, Polycyclic, SONATA
LoadAllPackages();

p:=2; l:=4; lambda:=2;

m:=l/lambda;
q:=p^l;
pi:=p^lambda;

Q:=FiniteField(q);
P:=FiniteField(pi);
H:=ElementaryAbelianGroup(q);

S:=GroupRing(Q, H);
embS:=Embedding(H, S);

R:=GroupRing(P, H);
embR:=Embedding(H, R);

HtoPcp:=IsomorphismPcpGroup(H);
qgens:=GeneratorsOfVectorSpace(Q);
sigma:=Random(SymmetricGroup(l));
qqgens:=Permuted(qgens, sigma);

phi:=function(h)
	return Exponents(Image(HtoPcp, h)) * qqgens;
end;

u:=function(i)
	local sum, h;
	sum:=Zero(S);

	for h in Elements(H) do
		sum:=sum + (phi(h)^i) * Image(embS, h);
	od;

	return sum;
end;

weight:=function(i)
	return Sum(CoefficientsQadic(i, pi));
end;

M:=function(k)
	local gens, i;
	gens:=[u(0)];

	for i in [1..q-1] do
		if weight(i) <= k then
			Add(gens, u(i));
		fi;
	od;

	return Ideal(S, gens, "basis");
end;

tr:=function(a)
	return Trace(Q, P, a);
end;

Tr:=function(a)
	if a = Zero(S) then
		return Zero(R);
	fi;
	return List(CoefficientsBySupport(a), tr) * Image(embR, Support(a));
end;

TrForIdeals:=function(I)
	local gens, trgens, i;
	gens:=Basis(I);
	trgens:=[];

	for i in Elements(Q) do
		Append(trgens, List(gens * i, Tr));
	od;

	return Ideal(R, trgens);
end;

RM:=function(k)
	return TrForIdeals(M(k));
end;

_RM:=function(k)
	# A slower version of RM()
	local mgens, rmgens, i;
	mgens:=[u(0)];
	rmgens:=[];

	for i in [1..q-1] do
		if weight(i) <= k then
			Add(mgens, u(i));
		fi;
	od;

	for i in Elements(Q) do
		Append(rmgens, List(mgens * i, Tr));
	od;

	return Ideal(R, rmgens);
end;
