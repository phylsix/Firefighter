#!/usr/bin/env python
import math

colors = [
    '#a6cee3', '#1f78b4', '#b2df8a', '#33a02c', '#fb9a99', '#e31a1c',
    '#fdbf6f', '#ff7f00', '#dd1c77'
]
pType = [
    'X', 'h', 'e', '$\mu$', '$\gamma$', 'h0', 'h_HF', 'e$\gamma$_HF',
    'dSA $\mu$'
]


def delta_phi(m, n):
    return (m.phi() - n.phi() + math.pi) % (2 * math.pi) - math.pi


def delta_r(m, n):
    return math.hypot((m.eta() - n.eta()), delta_phi(m, n))


def formatPoint(p):
    return (round(p.X(), 3), round(p.Y(), 3), round(p.Z(), 3))
