# Synth-A-Modeler Designer

The Synth-A-Modeler project was carried out at the TU Berlin's Audio
Communication Group. The
[**Synth-A-Modeler compiler**](https://github.com/chairaudio/SaM) was
written by [Edgar Berdahl](http://edgarberdahl.com/), and the graphical user interface, the
[**Synth-A-Modeler Designer**](https://github.com/chairaudio/SaM-Designer) was
created by [Peter Vasil](http://www.petervasil.net/).
We would like to graciously thank Prof. Julius O. Smith III,
Alexandros Kontogeorgakopoulos, Prof. Stefan Weinzierl,
Prof. Yann Orlarey, and the Alexander von Humboldt foundation for their
support.

## Screenshot

![Synth-A-Modeler Designer](https://github.com/ptrv/SaM-Designer/raw/master/screenshot.png
 "Synth-A-Modeler Designer")


## Project structure

**SaM**: This folder consist of a forked *Synth-A-Modeler compiler* by Edgar Berdahl
  and example model files.

<!-- **cmd2**: A C++ version of the *Synth-A-Modeler* compiler (experimental). -->

**extras**: Consists of Synth-A-Modeler file editor support for Emacs
  and Vim and a *Synth-A-Modeler Designer* Linux installer.

**gui**: This folder consist of the *Synth-A-Modeler Designer* project
  files and sources.

## Setup

The Synth-A-ModelerGUI project is heavily using the [JUCE][1] library.

### Get the sources and dependencies

Install [Pure-lang](https://github.com/agraef/pure-lang) and [Faust](https://github.com/grame-cncm/faust) 

Clone the Synth-A-Modeler repository:

    git clone --recursive https://github.com/chairaudio/SaM-Designer.git
    
The `--recursive` switch tells git to pull also the JUCE ans SaM submodules.
Without the switch you have to seperately get the submodule.

    git clone https://github.com/chairaudio/SaM-Designer.git
    cd SaM-Designer
    git submodule update --init

### Compiling Projucer

Projucer is part of the JUCE library. It is the project manager for
JUCE projects. Every time you have to make changes to the
Synth-A-ModelerGUI projects (i.e. add new source files, change compile
options), you have to use the Proojucer application. But first it must
be compiled.

Depending on your development machine there are project files for several
platforms. For linux:

    cd SaM-Designer
    cd juce/extras/Projucer/Builds/LinuxMakefile
    make
    build/Projucer

With the Projucer you can now open `*.jucer` files, like the one at the
`SaM-Designer/gui` path.

### Compiling Synth-A-Modeler Designer

There are project files for different operating systems provided. For
Mac OS X a Xcode project, for Windows Visual Studio 2010 project and a
Makefile for Linux.

To compile on Linux:

    cd SaM-Designer
    cd gui/Builds/Linux
    make

Run the application:

    build/Synth-A-Modeler-Designer


[1]: https://juce.com

## Usage

Please refer to the [Wiki][3] for information on the usage of
*Synth-A-Modeler Designer*.

[3]: https://github.com/ptrv/Synth-A-Modeler/wiki
