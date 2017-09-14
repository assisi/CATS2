\NeedsTeXFormat{LaTeX2e}

\ProvidesClass{PPR}[2008/04/25 v1.0]

% Pass all options to report.cls
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{book}}

\ProcessOptions

\LoadClass[a4paper,twoside,openany]{book}

%-----------------------------------------------------------
\bibliographystyle{plain}




%If you need any packages just load them here:
\RequirePackage{titlesec}
\RequirePackage[british]{babel}
\RequirePackage{fancyhdr}
\RequirePackage[a4paper]{geometry}
\RequirePackage{amsmath}
\RequirePackage{lastpage}
\RequirePackage[dvips]{graphicx}
\RequirePackage{subfigure}
\RequirePackage{epsfig}
\RequirePackage{makeidx}
\RequirePackage{color}
\RequirePackage{lscape}
\RequirePackage{supertabular}
%\RequirePackage{xtab}
\RequirePackage{hhline}
\RequirePackage{rotating}
\RequirePackage{ifthen}
\RequirePackage{colortbl}
\RequirePackage{longtable}

\RequirePackage{tabularx}
\RequirePackage{float}
\RequirePackage{fixltx2e}

\doublerulesepcolor{white}%helps against a bug in colortbl with hhline

\setcounter{lofdepth}{2}

\pagestyle{fancy}
\fancyhead{}
\fancyhead[LO,RE]{FP7 GA No.601074~~~{\bf ASSISIbf}}
%\fancyfoot[LO,RE]{\DeliveryDate}
\fancyfoot[LO,RE]{\DelivName}
\fancyfoot[CO,CE]{}
\fancyfoot[RO,LE]{Page \thepage \hspace*{1pt} of \pageref{LastPage}}
\renewcommand{\footrulewidth}{0.4pt}

\fancypagestyle{plain}{
  \fancyhead{}
  \fancyhead[LO,RE]{FP7 GA No.601074~~~{\bf ASSISIbf}}
  %\fancyfoot[LO,RE]{Page \thepage}
  %\fancyfoot[RO,LE]{Page \thepage}
  %\fancyfoot[CO,RE]{\DelivName}
  %\fancyfoot[CO,CE]{}
  %\fancyfoot[RO,LE]{Page \thepage \hspace*{1pt} of \pageref{LastPage}}
  \renewcommand{\footrulewidth}{0.4pt}
}

% modify chapter, section, subsection headings
\titleformat{\chapter}[hang]{\huge\sf}{\thechapter.}{6pt}{}[]%\thispagestyle{fancy}]
\titleformat{\section}[hang]{\Large\sf}{\thesection}{4pt}{}[]
\titleformat{\subsection}[hang]{\large\sf}{\thesubsection}{2pt}{}[]
\titleformat{\subsubsection}[hang]{\sf}{\thesubsubsection.}{12pt}{}[]

\newcommand{\rotpi}[1]{%
 \begin{sideways}#1\end{sideways}
}

\newcommand{\hint}[1]{
  {\large\bf\textcolor[rgb]{1,0,0}{\textsf{#1}}}
}

%including graphics
%first parameter is the picture
%second parameter is the caption
%third parameter is the label
%fourth scaling factor
\newcommand{\includepic}[4]{
  \begin{figure}[htb]
    \centering
    \includegraphics[width=#4\textwidth]{#1}
    \caption{\label{#3}#2}
  \end{figure}
}

%including graphics without a caption
%first parameter is the picture
%second scaling factor
\newcommand{\includepicNC}[2]{
  \begin{figure}[htb]
    \centering
    \includegraphics[width=#2\textwidth]{#1}
  \end{figure}
}

%creates a text box with a frame around
%first parameter the width of the box
%second parameter the text
\newcommand{\textbox}[2]{
  \begin{tabular}[h]{|c|}
    \hline\\
    \begin{minipage}[h]{#1\textwidth}
      #2
    \end{minipage}\\\\
    \hline
  \end{tabular}
}

\newcommand{\wpnumber}[1]{
  \begin{minipage}{1.8cm}
    \vspace{2pt} 
    \centering
    \small #1
    \vspace{2pt} 
  \end{minipage}
}

\newcommand{\wpname}[1]{
  \begin{minipage}{3cm}
    \vspace{2pt} 
    \small #1
    \vspace{2pt} 
  \end{minipage}
}

\newcommand{\wptasknumber}[1]{
  \begin{minipage}{1.2cm}
    \vspace{2pt} 
    \centering
    \small #1
    \vspace{2pt} 
  \end{minipage}
}

\newcommand{\wptaskname}[1]{
  \begin{minipage}{3cm}
    \vspace{2pt} 
    \small #1
    \vspace{2pt} 
  \end{minipage}
}

\newcommand{\wptaskdesc}[1]{
  \begin{minipage}{9.7cm}
    \vspace{2pt} 
    \small #1
    \vspace{2pt} 
  \end{minipage}
}

% \newcommand{\wptabularentry}[5]{
%%   \wpnumber{#1}&
%%   \wpname{#2}&
%%   \wptasknumber{#3}&
%%   \wptaskname{#4}&
%%   \wptaskdesc{#5}\\ \hhline{||-|-|-|-|-||}
%% }
%% \newcommand{\wptabularentry}[6]{
%%   {\small #1} & {\small #2} & {\small #3} & \hspace*{-6pt}\begin{tabular}{p{3cm}} {\small
%%       #4}\\\\{\small Task~Leader: #5} \end{tabular} & {\small #6}\\\hhline{||-|-|-|-|-||}
%% }
\newcommand{\wptabularentry}[6]{
  {\small #1} & {\small #2} & {\small #3} & {\small #4 \linebreak Task~Leader: #5}
  & {\small #6} \\\hhline{||-|-|-|-|-||}
}

\newenvironment{wptable}{
%  \begin{tabular}{||l|l|l|l|l||}
  \begin{tabular}{||p{1.8cm}|p{3cm}|p{1.2cm}|p{3cm}|p{9.7cm}||}
    \hhline{|t:=====:t|}
    {\small Workpackage Number} & {\small Workpackage Name} & {\small Task Number}
    &{\small Task Name}&{\small Task description}\\ 
    \hhline{|:=====:|}
}
{
  \hhline{|b:=====:b|}
\end{tabular}

}
\newcommand{\delivnumber}[1]{
  \begin{minipage}{1.3cm}
    \vspace{2pt} 
    \small #1
    \vspace{2pt} 
  \end{minipage}
}

\newcommand{\delivtitle}[1]{
  \begin{minipage}{5.5cm}
    \vspace{2pt} 
    \small #1
    \vspace{2pt} 
  \end{minipage}
}

\newcommand{\delivpartner}[1]{
  \begin{minipage}{1.5cm}
    \vspace{2pt} 
    \small #1
    \vspace{2pt} 
  \end{minipage}
}

\newcommand{\delivdate}[1]{
  \begin{minipage}{1cm}
    \vspace{2pt} 
    \small #1
    \vspace{2pt} 
  \end{minipage}
}

\newcommand{\delivnature}[1]{
  \begin{minipage}{1.1cm}
    \vspace{2pt} 
    \small #1
    \vspace{2pt} 
  \end{minipage}
}

\newcommand{\delivlevel}[1]{
  \begin{minipage}{1.5cm}
    \vspace{2pt} 
    \small #1
    \vspace{2pt} 
  \end{minipage}
}

\newcommand{\delivtabularentry}[6]{
  \delivnumber{#1}&
  \delivpartner{#2}&
  \delivtitle{#3}&
  \delivdate{#4}&
  \delivnature{#5}&
  \delivlevel{#6}\\ \hhline{||-|-|-|-|-|-||}
}

\newenvironment{delivtable}{
  \begin{center}
    \begin{tabular}{||l|l|l|l|l|l||}
      \hhline{|t:======:t|}
      \delivtabularentry{\bf Deliver-able N$^\circ$
      }{\bf Partner}{\bf Deliverable title}
      {\bf Due Month
      }{\bf Nature
      }{\bf Diss-emination-level}
      \hhline{|:======:|}
}
{
  \hhline{|b:======:b|}
\end{tabular}
\end{center}
}

\newcommand{\milestnumber}[1]{
  \begin{minipage}{1.5cm}
    \vspace{2pt} 
    \small #1
    \vspace{2pt} 
  \end{minipage}
}

\newcommand{\milesttitle}[1]{
  \begin{minipage}{8cm}
    \vspace{2pt} 
    \small #1
    \vspace{2pt} 
  \end{minipage}
}

\newcommand{\milestpartner}[1]{
  \begin{minipage}{2cm}
    \vspace{2pt} 
    \small #1
    \vspace{2pt} 
  \end{minipage}
}

\newcommand{\milestdate}[1]{
  \begin{minipage}{1.3cm}
    \vspace{2pt} 
    \small #1
    \vspace{2pt} 
  \end{minipage}
}

\newcommand{\milestonetabularentry}[4]{
  \milestnumber{#1}&
  \milestpartner{#2}&
  \milesttitle{#3}&
  \milestdate{#4}\\ \hhline{||-|-|-|-||}
}

\newenvironment{milestonetable}{
  \begin{center}
    \begin{tabular}{||l|l|l|l||}
      \hhline{|t:====:t|}
      \milestonetabularentry{{\bf Milestone N$^\circ$
      }}{{\bf Partner (s)}}{{\bf Milestone description}}
      {{\bf Due Month}}
      \hhline{|:====:|}
}
{
  \hhline{|b:====:b|}
\end{tabular}
\end{center}
}

 \newcommand{\timesep}{
 \\ \hhline{|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|}
 }

\newcommand{\timewphead}[2]{
\rowcolor[rgb]{0,.4,0} {\color{white}#1} & {\color{white}#2} &  &  &  &  &  &  &  &  &  &  &  &  & \timesep
}
\newcommand{\timetaskhead}[2]{
\rowcolor[rgb]{0.6,.8,0} #1 & #2 &  &  &  &  &  &  &  &  &  &  &  &  & \timesep
}

\newcommand{\timesubtaskYear}[3]{%
  {\bf #1} & {\bf #2} & {\bf #3} &  \multicolumn{12}{|>{\columncolor[gray]{0.6}}l|}{} \timesep 
}

\newcommand{\timesubtaskHalfYear}[3]{%
  {\bf#1} & {\bf #2} & {\bf #3} & \multicolumn{6}{|>{\columncolor[gray]{0.6}}l|}{} & \multicolumn{6}{l|}{} \timesep 
}

\newcommand{\timesubtaskLastHalfYear}[3]{%
  {\bf#1} & {\bf #2} & {\bf #3}  & \multicolumn{6}{l|}{} & \multicolumn{6}{|>{\columncolor[gray]{0.6}}l|}{}\timesep 
}

\newenvironment{timetable}{
  \begin{center}
    \begin{tabular}{|p{6cm}|c|p{2cm}|c|c|c|c|c|c|c|c|c|c|c|c|}
      \hline 
      {\bf Task/ Subtask/ Deliverable/ Milestone} & {\bf Responsible} & {\bf Partners}
      & \multicolumn{12}{l|}{{\bf Month}}\\
      &  &  & 19 & 20 & 21 & 22 & 23 & 24 & 25 & 26 & 27 & 28 & 29 & 30\\
      \hline
}
{
\end{tabular}
\end{center}
}

 \newcommand{\ftimesep}{
 \\ \hhline{|-|-|-|-|-|-|-|-|-|}
 }

\newcommand{\ftimewphead}[2]{
\rowcolor[rgb]{0,.4,0} {\color{white}#1} & {\color{white}#2} & &  &  &  &  &  &  \ftimesep
}
\newcommand{\ftimetaskhead}[2]{
\rowcolor[rgb]{0.6,.8,0} #1 & #2 &  &  &  &  &  &  & \ftimesep
}

\newcommand{\ftimesubtaskHalfYear}[3]{%
  {\bf #1} & {\bf #2} & {\bf #3} &  \multicolumn{6}{|>{\columncolor[gray]{0.6}}l|}{} \ftimesep 
}

\newcommand{\ftimesubtaskQuaterYear}[3]{%
  {\bf#1} & {\bf #2} & {\bf #3} & \multicolumn{3}{|>{\columncolor[gray]{0.6}}l|}{} & \multicolumn{3}{l|}{} \ftimesep 
}

\newcommand{\ftimesubtaskLastQuaterYear}[3]{%
  {\bf#1} & {\bf #2} & {\bf #3}  & \multicolumn{3}{l|}{} & \multicolumn{3}{|>{\columncolor[gray]{0.6}}l|}{}\ftimesep 
}

\newenvironment{ftimetable}{
  \begin{center}
    \begin{tabular}{|p{10.5cm}|c|p{2cm}|c|c|c|c|c|c|}
      \hline 
      {\bf Task/ Subtask/ Deliverable/ Milestone} & {\bf Responsible} & {\bf Partners}
      & \multicolumn{6}{l|}{{\bf Month}}\\
      &  &  & 13 & 14 & 15 & 16 & 17 & 18\\
      \hline
}
{
\end{tabular}
\end{center}
}

%add new enviroments here
\newenvironment{proposition}{%
  \vspace*{6pt}
 {\bf Proposition.}\it}{\vspace*{6pt}}%

\newenvironment{proof}{%
  \vspace*{6pt}
  {\it Proof.}\it}{\vspace*{6pt}}%

\newenvironment{corollary}{%
  \vspace*{6pt}
  {\bf Corollary.}\it}{\vspace*{6pt}}%

\newenvironment{lemma}{%
  \vspace*{6pt}
  {\bf Lemma.}\it}{\vspace*{6pt}}%

\newenvironment{definition}{%
  \vspace*{6pt}
  {\bf Definition.}\it}{\vspace*{6pt}}%

\newenvironment{theorem}[1]{%
  \vspace*{6pt}
  {\bf Theorem (#1).}\it}{\vspace*{6pt}}%

\newenvironment{example}[1]{%
  \vspace*{6pt}
  {\bf Example (#1).}\it}{\vspace*{6pt}}%

\def\squareforqed{\hbox{\rlap{$\sqcap$}$\sqcup$}}
\def\qed{\ifmmode\squareforqed\else{\unskip\nobreak\hfil
\penalty50\hskip1em\null\nobreak\hfil\squareforqed
\parfillskip=0pt\finalhyphendemerits=0\endgraf}\fi}

\def\getsto{\mathrel{\mathchoice {\vcenter{\offinterlineskip
\halign{\hfil
$\displaystyle##$\hfil\cr\gets\cr\to\cr}}}
{\vcenter{\offinterlineskip\halign{\hfil$\textstyle##$\hfil\cr\gets
\cr\to\cr}}}
{\vcenter{\offinterlineskip\halign{\hfil$\scriptstyle##$\hfil\cr\gets
\cr\to\cr}}}
{\vcenter{\offinterlineskip\halign{\hfil$\scriptscriptstyle##$\hfil\cr
\gets\cr\to\cr}}}}}
\def\lid{\mathrel{\mathchoice {\vcenter{\offinterlineskip\halign{\hfil
$\displaystyle##$\hfil\cr<\cr\noalign{\vskip1.2pt}=\cr}}}
{\vcenter{\offinterlineskip\halign{\hfil$\textstyle##$\hfil\cr<\cr
\noalign{\vskip1.2pt}=\cr}}}
{\vcenter{\offinterlineskip\halign{\hfil$\scriptstyle##$\hfil\cr<\cr
\noalign{\vskip1pt}=\cr}}}
{\vcenter{\offinterlineskip\halign{\hfil$\scriptscriptstyle##$\hfil\cr
<\cr
\noalign{\vskip0.9pt}=\cr}}}}}
\def\gid{\mathrel{\mathchoice {\vcenter{\offinterlineskip\halign{\hfil
$\displaystyle##$\hfil\cr>\cr\noalign{\vskip1.2pt}=\cr}}}
{\vcenter{\offinterlineskip\halign{\hfil$\textstyle##$\hfil\cr>\cr
\noalign{\vskip1.2pt}=\cr}}}
{\vcenter{\offinterlineskip\halign{\hfil$\scriptstyle##$\hfil\cr>\cr
\noalign{\vskip1pt}=\cr}}}
{\vcenter{\offinterlineskip\halign{\hfil$\scriptscriptstyle##$\hfil\cr
>\cr
\noalign{\vskip0.9pt}=\cr}}}}}
\def\grole{\mathrel{\mathchoice {\vcenter{\offinterlineskip
\halign{\hfil
$\displaystyle##$\hfil\cr>\cr\noalign{\vskip-1pt}<\cr}}}
{\vcenter{\offinterlineskip\halign{\hfil$\textstyle##$\hfil\cr
>\cr\noalign{\vskip-1pt}<\cr}}}
{\vcenter{\offinterlineskip\halign{\hfil$\scriptstyle##$\hfil\cr
>\cr\noalign{\vskip-0.8pt}<\cr}}}
{\vcenter{\offinterlineskip\halign{\hfil$\scriptscriptstyle##$\hfil\cr
>\cr\noalign{\vskip-0.3pt}<\cr}}}}}
\def\bbbr{{\rm I\!R}} %reelle Zahlen
\def\bbbm{{\rm I\!M}}
\def\bbbn{{\rm I\!N}} %natuerliche Zahlen
\def\bbbf{{\rm I\!F}}
\def\bbbh{{\rm I\!H}}
\def\bbbk{{\rm I\!K}}
\def\bbbp{{\rm I\!P}}
\def\bbbone{{\mathchoice {\rm 1\mskip-4mu l} {\rm 1\mskip-4mu l}
{\rm 1\mskip-4.5mu l} {\rm 1\mskip-5mu l}}}
\def\bbbc{{\mathchoice {\setbox0=\hbox{$\displaystyle\rm C$}\hbox{\hbox
to0pt{\kern0.4\wd0\vrule height0.9\ht0\hss}\box0}}
{\setbox0=\hbox{$\textstyle\rm C$}\hbox{\hbox
to0pt{\kern0.4\wd0\vrule height0.9\ht0\hss}\box0}}
{\setbox0=\hbox{$\scriptstyle\rm C$}\hbox{\hbox
to0pt{\kern0.4\wd0\vrule height0.9\ht0\hss}\box0}}
{\setbox0=\hbox{$\scriptscriptstyle\rm C$}\hbox{\hbox
to0pt{\kern0.4\wd0\vrule height0.9\ht0\hss}\box0}}}}
\def\bbbq{{\mathchoice {\setbox0=\hbox{$\displaystyle\rm
Q$}\hbox{\raise
0.15\ht0\hbox to0pt{\kern0.4\wd0\vrule height0.8\ht0\hss}\box0}}
{\setbox0=\hbox{$\textstyle\rm Q$}\hbox{\raise
0.15\ht0\hbox to0pt{\kern0.4\wd0\vrule height0.8\ht0\hss}\box0}}
{\setbox0=\hbox{$\scriptstyle\rm Q$}\hbox{\raise
0.15\ht0\hbox to0pt{\kern0.4\wd0\vrule height0.7\ht0\hss}\box0}}
{\setbox0=\hbox{$\scriptscriptstyle\rm Q$}\hbox{\raise
0.15\ht0\hbox to0pt{\kern0.4\wd0\vrule height0.7\ht0\hss}\box0}}}}
\def\bbbt{{\mathchoice {\setbox0=\hbox{$\displaystyle\rm
T$}\hbox{\hbox to0pt{\kern0.3\wd0\vrule height0.9\ht0\hss}\box0}}
{\setbox0=\hbox{$\textstyle\rm T$}\hbox{\hbox
to0pt{\kern0.3\wd0\vrule height0.9\ht0\hss}\box0}}
{\setbox0=\hbox{$\scriptstyle\rm T$}\hbox{\hbox
to0pt{\kern0.3\wd0\vrule height0.9\ht0\hss}\box0}}
{\setbox0=\hbox{$\scriptscriptstyle\rm T$}\hbox{\hbox
to0pt{\kern0.3\wd0\vrule height0.9\ht0\hss}\box0}}}}
\def\bbbs{{\mathchoice
{\setbox0=\hbox{$\displaystyle     \rm S$}\hbox{\raise0.5\ht0\hbox
to0pt{\kern0.35\wd0\vrule height0.45\ht0\hss}\hbox
to0pt{\kern0.55\wd0\vrule height0.5\ht0\hss}\box0}}
{\setbox0=\hbox{$\textstyle        \rm S$}\hbox{\raise0.5\ht0\hbox
to0pt{\kern0.35\wd0\vrule height0.45\ht0\hss}\hbox
to0pt{\kern0.55\wd0\vrule height0.5\ht0\hss}\box0}}
{\setbox0=\hbox{$\scriptstyle      \rm S$}\hbox{\raise0.5\ht0\hbox
to0pt{\kern0.35\wd0\vrule height0.45\ht0\hss}\raise0.05\ht0\hbox
to0pt{\kern0.5\wd0\vrule height0.45\ht0\hss}\box0}}
{\setbox0=\hbox{$\scriptscriptstyle\rm S$}\hbox{\raise0.5\ht0\hbox
to0pt{\kern0.4\wd0\vrule height0.45\ht0\hss}\raise0.05\ht0\hbox
to0pt{\kern0.55\wd0\vrule height0.45\ht0\hss}\box0}}}}
\def\bbbz{{\mathchoice {\hbox{$\mathsf\textstyle Z\kern-0.4em Z$}}
{\hbox{$\mathsf\textstyle Z\kern-0.4em Z$}}
{\hbox{$\mathsf\scriptstyle Z\kern-0.3em Z$}}
{\hbox{$\mathsf\scriptscriptstyle Z\kern-0.2em Z$}}}}


\endinput
