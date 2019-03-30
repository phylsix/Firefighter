#!/usr/bin/env python
import math

colors = [
    "#a6cee3",
    "#1f78b4",
    "#b2df8a",
    "#33a02c",
    "#fb9a99",
    "#e31a1c",
    "#fdbf6f",
    "#ff7f00",
    "#dd1c77",
]
pType = ["X", "h", "e", "$\mu$", "$\gamma$", "h0", "h_HF", "e$\gamma$_HF", "dSA $\mu$"]


def delta_phi(m, n):
    return (m.phi() - n.phi() + math.pi) % (2 * math.pi) - math.pi


def delta_r(m, n):
    return math.hypot((m.eta() - n.eta()), delta_phi(m, n))


def formatPoint(p, precision=3):
    return (round(p.X(), precision), round(p.Y(), precision), round(p.Z(), precision))


def formatEtaPhi(p, precision=3):
    return (round(p.eta(), precision), round(p.phi(), precision))
