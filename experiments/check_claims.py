#!/usr/bin/env python3
"""Claims-vs-artifacts lint for the manuscript (menu item 5).

Scans doc/manuscript.tex for quantitative claims (numbers, ratios, percentages,
powers of ten) and links each one to an artefact that could support it: a note,
a script, a CSV or a log under experiments/, quantization/ or build/.  A number
that appears only in the manuscript's own prose is flagged as prose-only, i.e.
not independently recomputable from the repository.

No third-party packages: numpy/pandas are not installed in this environment.

Usage: python experiments/check_claims.py [--top N] [--md out.md]
"""

import argparse
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MANUSCRIPT = os.path.join(ROOT, "doc", "manuscript.tex")

# Directories whose files count as artefacts (the manuscript itself excluded).
ARTIFACT_DIRS = ["experiments", "quantization", "build"]
ARTIFACT_EXT = {".md", ".py", ".txt", ".csv", ".ps1", ".bat", ".tex", ".cpp", ".h"}
SKIP_FILES = {"manuscript.tex"}

# A number-like token, tolerating thousands separators and decimals.
NUM = re.compile(r"\d[\d,]*(?:\.\d+)?")


def strip_latex(s):
    """Drop comments and thin-space commands so numbers can be matched."""
    s = re.sub(r"(?<!\\)%.*", "", s)
    for a, b in ((r"\\,", " "), (r"\\;", " "), (r"\\ ", " "), (r"~", " ")):
        s = s.replace(a, b)
    return s


def normalise(tok):
    return tok.replace(",", "").rstrip(".")


STOPWORDS = {
    "the", "and", "with", "that", "this", "from", "which", "where", "their",
    "there", "these", "those", "than", "then", "when", "will", "would", "been",
    "have", "has", "are", "was", "were", "for", "not", "but", "its", "into",
    "only", "each", "same", "such", "both", "also", "more", "most", "some",
    "section", "figure", "table", "below", "above", "reported", "building",
}


def is_claim_number(n):
    """Keep tokens that can carry a measurement, drop narrative years.

    A calendar year in running text is a citation/history marker, not a claim;
    everything with four or more digits, or with a decimal or thousands
    separator, is kept.
    """
    digits = n.replace(".", "").replace(",", "")
    if n.count(".") == 0 and "," not in n and len(digits) == 4:
        if 1900 <= int(digits) <= 2035:
            return False
    return len(digits) >= 4 or "." in n or "," in n


def words_of(text):
    """Content words (>=6 letters, lowercase) used as the context signature."""
    return {w for w in re.findall(r"[A-Za-z]{6,}", text.lower())
            if w not in STOPWORDS}


def load_artifacts():
    """rel_path -> (numbers, content words) of that artefact file.

    Manuscript backups inside build/ are excluded: they repeat the manuscript's
    own numbers, so counting them as artefacts would be circular.
    """
    corpus = {}
    for d in ARTIFACT_DIRS:
        base = os.path.join(ROOT, d)
        if not os.path.isdir(base):
            continue
        for dirpath, _dirs, files in os.walk(base):
            for f in files:
                low = f.lower()
                # skip the manuscript backups, this tool's generated table and
                # its own note (which quotes the manuscript's numbers by design)
                if (f in SKIP_FILES or "manuscript" in low
                        or low.startswith("claims_lint")):
                    continue
                if os.path.splitext(f)[1].lower() not in ARTIFACT_EXT:
                    continue
                path = os.path.join(dirpath, f)
                try:
                    if os.path.getsize(path) > 8 * 1024 * 1024:
                        continue
                    with open(path, "r", encoding="utf-8", errors="replace") as fh:
                        text = fh.read()
                except OSError:
                    continue
                rel = os.path.relpath(path, ROOT).replace("\\", "/")
                nums = {normalise(m.group(0)) for m in NUM.finditer(text)}
                corpus[rel] = (nums, words_of(text))
    return corpus


def artifact_index(corpus):
    """number -> sorted list of artefact files containing it (>=3 digits only)."""
    idx = {}
    for rel in sorted(corpus):
        for n in corpus[rel][0]:
            if len(n) >= 3:
                idx.setdefault(n, []).append(rel)
    return idx


def sentences(text):
    """Yield (line_number, sentence) pairs, joining wrapped LaTeX lines."""
    buf, start = "", 0
    for i, raw in enumerate(text.splitlines(), 1):
        line = strip_latex(raw)
        if not buf:
            start = i
        buf += " " + line
        if re.search(r"[.!?]\s*$", line) or len(buf) > 1200:
            yield start, " ".join(buf.split())
            buf = ""
    if buf.strip():
        yield start, " ".join(buf.split())


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--top", type=int, default=40)
    ap.add_argument("--md", default="")
    args = ap.parse_args()

    if not os.path.isfile(MANUSCRIPT):
        print("manuscript not found: %s" % MANUSCRIPT)
        return 2

    with open(MANUSCRIPT, "r", encoding="utf-8", errors="replace") as fh:
        text = fh.read()
    cut = text.find("\\begin{thebibliography}")
    if cut > 0:
        text = text[:cut]          # citation years and page numbers are not claims

    corpus = load_artifacts()
    idx = artifact_index(corpus)

    strong, weak, prose_only = [], [], []
    for line_no, sent in sentences(text):
        if any(t in sent for t in ("\\usepackage", "\\documentclass", "PACS numbers")):
            continue
        nums = {normalise(m.group(0)) for m in NUM.finditer(sent)}
        nums = {n for n in nums if is_claim_number(n)}
        if not nums:
            continue
        ctx = words_of(sent)
        num_hits, strong_hits = {}, {}
        for n in sorted(nums):
            files = idx.get(n)
            if not files:
                continue
            num_hits[n] = files[:3]
            good = [f for f in files if len(ctx & corpus[f][1]) >= 2]
            if good:
                strong_hits[n] = good[:3]
        if strong_hits:
            strong.append((line_no, sent, strong_hits))
        elif num_hits:
            weak.append((line_no, sent, num_hits))
        else:
            prose_only.append((line_no, sent, sorted(nums)))

    total = len(strong) + len(weak) + len(prose_only)
    print("CLAIMS LINT")
    print("artefact files scanned: %d" % len(corpus))
    print("sentences carrying a >=3-digit number: %d" % total)
    print("  STRONG  number + >=2 shared content words -> an artefact: %d" % len(strong))
    print("  WEAK    number appears in an artefact, no shared context: %d" % len(weak))
    print("  NONE    number absent from every artefact (prose-only): %d" % len(prose_only))
    print()
    print("--- PROSE-ONLY (must be recomputed or removed) first %d" % args.top)
    for line_no, sent, nums in prose_only[:args.top]:
        print("L%-6d %s" % (line_no, sent[:185]))
        print("        numbers: %s" % ", ".join(nums))
    print()
    print("--- WEAK (number-only match; context differs) first %d" % args.top)
    for line_no, sent, hits in weak[:args.top]:
        n0 = list(hits.items())[0]
        print("L%-6d %s" % (line_no, sent[:185]))
        print("        %s -> %s" % (n0[0], ", ".join(n0[1])))
    print()
    print("--- STRONG sample (number and context agree)")
    for line_no, sent, hits in strong[:6]:
        n0 = list(hits.items())[0]
        print("L%-6d %s" % (line_no, sent[:110]))
        print("        %s -> %s" % (n0[0], ", ".join(n0[1])))

    if args.md:
        out = os.path.join(ROOT, args.md)
        with open(out, "w", encoding="utf-8") as fh:
            fh.write("# Claims-vs-artifacts lint (generated by "
                     "`experiments/check_claims.py`)\n\n")
            fh.write("Artefact files scanned: **%d**.  Sentences carrying a "
                     ">=3-digit number: **%d** -- STRONG **%d**, WEAK **%d**, "
                     "NONE **%d**.\n\n"
                     % (len(corpus), total, len(strong), len(weak), len(prose_only)))
            for title, rows, kind in (("Prose-only (no artefact carries the number)",
                                       prose_only, "nums"),
                                      ("Weak (number only, context differs)",
                                       weak, "hits")):
                fh.write("## %s\n\n| line | claim | numbers / artefact |\n"
                         "|---|---|---|\n" % title)
                limit = args.top * 3 if kind == "nums" else args.top * 2
                for line_no, sent, data in rows[:limit]:
                    if kind == "nums":
                        detail = ", ".join(data)
                    else:
                        detail = "; ".join("%s -> %s" % (n, f[0])
                                           for n, f in list(data.items())[:2])
                    fh.write("| %d | %s | %s |\n"
                             % (line_no, sent[:160].replace("|", "/"), detail))
            fh.write("\n## Strong (number and context agree), sample\n\n"
                     "| line | claim | number -> artefact |\n|---|---|---|\n")
            for line_no, sent, hits in strong[:60]:
                n0 = list(hits.items())[0]
                fh.write("| %d | %s | %s -> %s |\n"
                         % (line_no, sent[:140].replace("|", "/"),
                            n0[0], n0[1][0]))
        print()
        print("markdown written: %s" % out)
    return 0

    if args.md:
        out = os.path.join(ROOT, args.md)
        with open(out, "w", encoding="utf-8") as fh:
            fh.write("# Claims-vs-artifacts lint (generated by "
                     "`experiments/check_claims.py`)\n\n")
            fh.write("Artefact files scanned: **%d**.  Sentences carrying a "
                     ">=3-digit number: **%d** (%d linked to an artefact, "
                     "**%d prose-only**).\n\n"
                     % (len(corpus), len(linked) + len(prose_only),
                        len(linked), len(prose_only)))
            fh.write("## Prose-only claims (no artefact carries the number)\n\n")
            fh.write("| line | claim | numbers |\n|---|---|---|\n")
            for line_no, sent, nums in prose_only[:args.top * 3]:
                fh.write("| %d | %s | %s |\n"
                         % (line_no, sent[:170].replace("|", "/"),
                            ", ".join(nums)))
            fh.write("\n## Linked claims (sample)\n\n")
            fh.write("| line | claim | number -> artefact |\n|---|---|---|\n")
            for line_no, sent, hits in linked[:60]:
                pairs = "; ".join("%s -> %s" % (n, files[0])
                                  for n, files in list(hits.items())[:2])
                fh.write("| %d | %s | %s |\n"
                         % (line_no, sent[:140].replace("|", "/"), pairs))
        print()
        print("markdown written: %s" % out)
    return 0


if __name__ == "__main__":
    sys.exit(main())
