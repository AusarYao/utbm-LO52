# -*- coding:Utf-8 -*-

def Sum_pow2(n):
		L = [2**i for i in range( 0, n + 1) if 2**i <= n]
		L.reverse()
		S = 0
		L2 = []
		while S < n:
			for i in L:
			    while ( S + i ) <= n:
			        S += i
			        L2.append( i )
			if L == [] :
			    return "N'est pas decomposable en somme de carres d'entiers"
		return L2

def Search_in_liste(L,value):
    for i in L:
        if i == value:
            return True
    return False
