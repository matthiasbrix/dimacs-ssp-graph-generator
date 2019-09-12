# dimacs-ssp-graph-generator

## Synopsis

This repository provides a program to generate graphs for the shortest path problem in the DIMACS SSP file format. One can simply insert the desired number of vertices and density of the graphs

## How to use

```
./dimacs.sh -n 100 -d 0.7 -w 100 1000
```
where `n` is the number of vertices, `d` the density of the constructed graph and `w` takes as first argument the minimum weight and as second the maximum weight. 

## Installation

```
git clone https://github.com/matthiasbrix/dimacs-ssp-graph-generator.git
```

## TODO

* Try Wilson's algorithm for producing the MST
* Add a CUDA implementation.

## Contributors

Matthias Brix
