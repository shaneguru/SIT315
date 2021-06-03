# Overview
For my module 4 task 1 which is the project, I was interested in exploring MPI furthermore, I have often experienced that when plotting data on maps, it becomes quite slow when the data
size increases, which eventually leads to issues with overall responsiveness. To try and tackle this problem, I decided to implement the visualization parallelly using the
mpi4py and bigmpi4py which are message passing libraries for python. Finally, I wanted to evaluate its effectiveness by comparing the MPI version and the sequential version.

# Design
This project was done using python mainly because of the plotly library which is an interactive open-source
graphing library which allowed plotting on beautiful maps.
Moving on, the other two main libraries used was mpi4py and bigmpi4py. Mpi4py is the standard MPI
library for python and bigmpi4py was a modified version which allowed to pass
pandas dataframe objects. All of these was used in the implementation to create the maps.

#Evaluation
