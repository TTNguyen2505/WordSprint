# WordSprint Report

This directory contains the Vietnamese academic report for WordSprint. The report is assembled with XeLaTeX from `main.tex`, which includes the front matter, chapters, references, and appendices. The build output is `main.pdf`.

The formatting and institutional details are configured in this project. Confirm any requirements with your instructor or institution before submission; this document does not certify compliance with external formatting standards.

## Get the project

Clone the WordSprint repository, then work from the report directory:

```powershell
git clone https://github.com/TTNguyen2505/WordSprint.git
cd WordSprint\report
```

If you already cloned the repository, run `cd report` from its root.

## Requirements

- Windows with a TeX distribution that provides **XeLaTeX** and **BibTeX**. [MiKTeX](https://miktex.org/download) is one option.
- The LaTeX packages used by the document. With MiKTeX, enable automatic installation of missing packages in MiKTeX Console, or install missing packages when prompted.
- A PDF viewer. If a viewer locks `main.pdf` and prevents it from being rewritten, close the viewer during compilation or use a viewer that supports file reload, such as a browser or SumatraPDF.

Perl is **not** required for the manual build commands below. It is only needed if you choose to install and use a Perl-based tool such as `latexmk`.

Check that the required commands are available in PowerShell:

```powershell
xelatex --version
bibtex --version
```

Run the build commands from this directory so that the relative paths to the chapters, assets, and bibliography resolve correctly.

## Build the report

Run XeLaTeX, BibTeX, and XeLaTeX twice to update citations, references, the table of contents, and lists. The exit-code checks stop the build if a step fails:

```powershell
xelatex -interaction=nonstopmode -halt-on-error -file-line-error main.tex
if ($LASTEXITCODE -ne 0) { throw "XeLaTeX pass 1 failed." }

bibtex main
if ($LASTEXITCODE -ne 0) { throw "BibTeX failed." }

xelatex -interaction=nonstopmode -halt-on-error -file-line-error main.tex
if ($LASTEXITCODE -ne 0) { throw "XeLaTeX pass 2 failed." }

xelatex -interaction=nonstopmode -halt-on-error -file-line-error main.tex
if ($LASTEXITCODE -ne 0) { throw "XeLaTeX pass 3 failed." }
```

The output is `main.pdf` in this directory. If a build fails, check the first error in the LaTeX output; later errors may be consequences of that first failure.

### Clean generated files

To remove common intermediate files while keeping the source and `main.pdf`, run this command from `report`:

```powershell
Remove-Item -Force -ErrorAction SilentlyContinue main.aux, main.bbl, main.blg, main.lof, main.log, main.lol, main.lot, main.out, main.toc, main.synctex.gz, "main.synctex(busy)"
```

## Project structure

```text
report/
├── main.tex                       # Master document
├── main.pdf                       # Compiled report
├── assets/
│   ├── figures/ch01/               # Chapter 1 figures
│   ├── logos/                      # Institutional logo
│   └── tables/ch01/                # Example table
├── backmatter/
│   ├── appendices/                 # Source-code and survey appendices
│   └── references.bib              # BibTeX references (IEEE style)
├── chapters/                       # Introduction through conclusion
├── config/
│   ├── abbreviations.tex
│   ├── format.sty                  # Layout and formatting
│   ├── metadata.tex                # Report details and author roster
│   └── packages.sty                # LaTeX packages and page setup
└── frontmatter/                    # Covers, review, declarations, and lists
```

## Customize the report

### Report details and authors

Edit `config/metadata.tex` before compiling. Replace the sample course, title, supervisor, date, class, cohort, author names, student IDs, assigned tasks, and contribution percentages with the correct information. For example:

```latex
\subject{Course name}
\reporttitle{Report title}
\subtitle{Project title}
\supervisor{Supervisor's name}
\submissiondate{Submission date}
\studentclass{Class}
\cohort{Cohort}

\addauthor{1}{Full Name}{Student ID}{Assigned task}{20\%}
```

The author roster is used by the cover, declaration, and task-assignment pages. Update or remove the sample author entries as needed.

### Chapters, references, and assets

- Edit the `.tex` files in `chapters/` and `frontmatter/` to update report content.
- Add bibliography entries to `backmatter/references.bib` and cite them from the chapters. The master document uses the `IEEEtran` BibTeX style.
- Store figures, logos, and tables under `assets/`. Use paths relative to this directory when including them in LaTeX.
- Update `config/format.sty` and `config/packages.sty` only when changing document formatting or package requirements.

## Licensing

There is no license file in this directory. Confirm the repository's licensing and any required template attribution with the project maintainers before redistributing this report template.
