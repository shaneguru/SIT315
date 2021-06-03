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

# Evaluation
![Screenshot 2021-06-03 190950](https://user-images.githubusercontent.com/42903047/120619668-88450180-c49f-11eb-82ae-ae67dd870353.jpg)

The above graph displays the evaluation between the sequential and mpi version of plotting
two maps. In the graph the y-axis is the execution time in seconds and the x-axis is the amount
of data which is plotted. In the graph we can clearly see that the mpi version performed better
when loading up the maps than the sequential version and the performance of mpi was
gradually getting better as the data size increased except for mpi with 2 processers, I believe
this was because the amount of data 1 process got was still a bit too high when it was divided
by 2. Moreover, it was also seen that running mpi with 4 processors was ideal for this range of
data.
