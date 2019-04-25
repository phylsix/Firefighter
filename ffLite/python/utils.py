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


def formatP4(p4, precisioin=3):
    res = "<{}, {}, {}, {}>".format(
        round(p4.px(), precisioin),
        round(p4.py(), precisioin),
        round(p4.pz(), precisioin),
        round(p4.energy(), precisioin),
    )
    return res

def formatP3(p3, precisioin=3):
    res = "<{}, {}, {}>".format(
        round(p3.px(), precisioin),
        round(p3.py(), precisioin),
        round(p3.pz(), precisioin),
    )
    return res


def getMomId(par):
    if par.numberOfMothers() > 0:
        return par.mother(0).pdgId()
    else:
        return "*"


def inferTypeFromPid(pid):
    """
    Given a pid, return a paritleType, ref - `pType`

    Parameters
    ----------
    pid : int
        paritle pdgId

    Returns
    -------
    int
        particle type
    """

    abspid = abs(pid)
    res = 0
    if abspid == 11:
        res = 2
    elif abspid == 13:
        res = 3
    elif abspid == 22:
        res = 4
    elif abspid in [111, 130, 310, 311]:  # pi0, Kl0, Ks0, K0
        res = 5
    elif abspid in [211, 321]:  # pi+, K+
        res = 1
    else:
        res = 0
    return res
