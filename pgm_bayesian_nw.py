def cpdA(A): return 0.01 if A else 0.99
def cpdS(S): return 0.5
def cpdT(T,A):
    if T: return 0.05 if A else 0.01
    else: return 0.95 if A else 0.99
def cpdL(L,S):
    if L: return 0.1 if S else 0.01
    else: return 0.9 if S else 0.99
def cpdB(B,S):
    if B: return 0.6 if S else 0.3
    else: return 0.4 if S else 0.7
def cpdE(E,L,T):
    if E: return 1 if L else (1 if T else 0)
    else: return 0 if L else (0 if T else 1)
def cpdX(X,E):
    if X: return 0.98 if E else 0.05
    else: return 0.02 if E else 0.95
def cpdD(D,E,B):
    if D:
        if (E and B): return 0.9
        if (E and not B): return 0.7
        if (not E and B): return 0.8
        return 0.1
    else:
        if (E and B): return 0.1
        if (E and not B): return 0.3
        if (not E and B): return 0.2
        return 0.9

def P(A,S,T,L,B,E,X,D):
    return cpdA(A) * cpdS(S) * cpdT(T,A) * cpdL(L,S) * cpdB(B,S) * cpdE(E,L,T) * cpdX(X,E) * cpdD(D,E,B)

# calculate P(D = true | X = true)
import itertools

d=True
x=True
p_d_x = 0
p_x = 0

combos = [[True,False]]*6
for item in itertools.product(*combos):
    p_d_x += P(*item,x,d)
    for D in [True,False]:
        p_x += P(*item,x,D)

print(p_d_x)
print(p_x)
p_d_cond_x = p_d_x/p_x
print(p_d_cond_x)
