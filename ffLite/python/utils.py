#!/usr/bin/env python
import math


def delta_phi(m, n):
    return (m.phi() - n.phi() + math.pi) % (2*math.pi) - math.pi


def delta_r(m, n):
    return math.hypot((m.eta() - n.eta()), delta_phi(m, n))


def formatPoint(p):
    return (
        round(p.X(), 3),
        round(p.Y(), 3),
        round(p.Z(), 3)
    )
