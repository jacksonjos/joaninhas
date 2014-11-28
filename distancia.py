#!/usr/bin/python2.7
# -*- coding: utf8 -*-

import math

def distance(line_1, col_1, line_2, col_2):
	parity_1 = line_1 % 2
	parity_2 = line_2 % 2

	if parity_1 == parity_2:
		print math.sqrt(abs(col_1-col_2)**2 + 0.75*(abs(line_1-line_2)*abs(line_1-line_2)))
	elif parity_1 == 0:
		print math.sqrt(abs(col_1-col_2+0.5)**2 + 0.75*(abs(line_1-line_2)*abs(line_1-line_2)))			
	elif parity_1 == 1:
		print math.sqrt(abs(col_1-col_2-0.5)**2 + 0.75*(abs(line_1-line_2)*abs(line_1-line_2)))			



while (True):
	valores = raw_input('digite índices i e j separados por espaços para dois vetores\n')
	a = []
	for e in valores.split():
		a.append(int(e))
	distance(a[0], a[1], a[2], a[3])