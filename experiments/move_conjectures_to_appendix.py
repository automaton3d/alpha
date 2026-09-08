#!/usr/bin/env python3
"""One-shot restructuring helper for doc/manuscript.tex (lote 6).

Moves the sections
  "Particles" (\\label{sec:Particles}) and
  "Cellular automata and quantum formalism" (\\label{sec:bridge})
to the end of the appendices (just before \\end{document}), and updates the
introduction's roadmap sentence.  Idempotency guard: exits 0 without changes
if the moves already happened (the headings then appear only after the last
\\section of the appendices / no occurrence before \\appendix).
"""
import re
import sys

PATH = "doc/manuscript.tex"
body = open(PATH, encoding="utf-8", errors="replace").read()
lines = body.splitlines(keepends=True)

def line_index(pat):
    rx = re.compile(pat)
    for i, ln in enumerate(lines):
        if rx.search(ln):
            return i
    return -1

sec_particles = r"\\section\{Particles\\label\{sec:Particles\}\}"
sec_bridge = r"\\section\{Cellular automata and quantum formalism\}.*?\\label\{sec:bridge\}"
next_dyn = r"\\section\{Dynamic charge quantization and the scaling of \$W\$\}"
next_conj = r"\\section\{Conjectures and prospects\\label\{sec:Prospects-and-conjectures\}\}"
appendix_marker = r"^\\appendix$"
end_doc = r"^\\end\{document\}$"

# Guard: headings must appear before \appendix already? We detect that a
# move was done if no occurrence is found before \appendix.
app_i = line_index(appendix_marker)
i_p = line_index(sec_particles)
i_b = line_index(sec_bridge)
if app_i < 0:
    raise SystemExit("no \\appendix marker found")
if not (0 <= i_p < app_i) or not (0 <= i_b < app_i):
    print("headings already moved or not found in the body; nothing to do")
    sys.exit(0)

def extract(start_pat, end_pat):
    """Return (block, new_lines) removing lines from start match to line
    before end_pat match (end_pat not included)."""
    s = line_index(start_pat)
    e = line_index(end_pat)
    if s < 0 or e < 0 or e <= s:
        raise SystemExit(f"cannot locate block {start_pat}..{end_pat} ({s},{e})")
    block = "".join(lines[s:e])
    new = lines[:s] + lines[e:]
    return block, new

# order matters: remove particles first, then bridge (line indexes recomputed
# each time because the file shifts).
bp, lines = extract(sec_particles, next_dyn)
bb, lines = extract(sec_bridge, next_conj)

# Adjust roadmap paragraph in the introduction.
roadmap_old = ("Sections~\\ref{sec:Interactions} and~\\ref{sec:Particles} detail "
               "bubble interactions and composite particles.")
roadmap_new = ("Section~\\ref{sec:Interactions} details bubble interactions; the "
               "composite-particle taxonomy is deferred to Appendix~\\ref{sec:Particles}.")
body2 = "".join(lines)
if roadmap_old in body2:
    body2 = body2.replace(roadmap_old, roadmap_new, 1)
roadmap2_old = ("Section~\\ref{sec:bridge} discusses the bridge to the "
                "quantum-mechanics formalism,")
roadmap2_new = ("The speculative bridge to the quantum-mechanics formalism is "
                "deferred to Appendix~\\ref{sec:bridge},")
if roadmap2_old in body2:
    body2 = body2.replace(roadmap2_old, roadmap2_new, 1)

# Reinsert at the end of the appendices, before \end{document}.
end_i = body2.find("\\end{document}")
if end_i < 0:
    raise SystemExit("\\end{document} not found")
insert = "\n\n% ---- moved by lote 6 (conjecture-labelled appendices) ----\n" \
         + bp.rstrip("\n") + "\n\n" + bb.rstrip("\n") + "\n"
body2 = body2[:end_i] + insert + "\n" + body2[end_i:]

open(PATH, "w", encoding="utf-8", newline="").write(body2)
print("moved Particles and CA/QM blocks to the appendices")
