#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <omp.h>
#include<CL/cl.h>

#define DIM 1000 //defining matrix size
#define MAX_RAND 5 //max value random generator

/* holding the matrices */
int matrix1[DIM][DIM];
int matrix2[DIM][DIM];
int res[DIM][DIM];
cl_mem bufA, bufB, bufC;

cl_device_id device_id;
cl_context context;
cl_program program;
cl_kernel kernel; //this is your kernel function
cl_command_queue  queue;
cl_event event = NULL;

int err;

const int max = DIM;
const int TS = 4;
const size_t local[2] = { (size_t)TS, (size_t)TS };
const size_t global[2] = { (size_t) max, (size_t)max }; 

cl_device_id create_device();
void setup_openCL_device_context_queue_kernel(char* filename, char* kernelname);
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename);
void setup_kernel_memory();
void copy_kernel_args();
void free_memory();

/* declaring function used */
void head(int proc, int start, int end);
void node(int proc, int start, int end);
void create(int matrix[DIM][DIM]);
void output(int matrix[DIM][DIM]);
void del(int matrix[DIM][DIM]);


struct vari
{
    int proc_rank, proc, start, end, num_threads;
    double t_start, t_stop;
};

vari val;

int main(int argc, char *argv[])
{


    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &val.proc_rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &val.proc);    

    val.num_threads = 1;
    val.start = val.proc_rank * DIM / val.proc;
    val.end = ((val.proc_rank + 1) * DIM / val.proc);

    if (val.proc_rank == 0){
        val.t_start = omp_get_wtime();
    }

    if (val.proc_rank == 0)
    {
        head(val.proc, val.start, val.end);

    }
    else
    {
        node(val.proc, val.start, val.end);
    }

    if (val.proc_rank == 0)
    {

    }

    if (val.proc_rank == 0){
        val.t_stop= omp_get_wtime();
        printf("MPI Matrix Multiplication Performance with OpenCL\n");
        printf("Dimension: %d \n", DIM);
        printf("Processes: %d \n", val.proc);
        printf("Threads: %d \n", val.num_threads);
        printf("Run time: %f \n", val.t_stop-val.t_start);
        if(DIM<=10){
            printf("First matrix: \n");
            output(matrix1);
            printf("Second matrix: \n"); 
            output(matrix2);
            printf("Result: \n");  
            output(res);
        }

        free_memory();

    }

    
    MPI_Finalize();
    return 0;
}

void head(int proc, int start, int end){
    create(matrix1); create(matrix2);

    MPI_Bcast(matrix2, DIM * DIM, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(&matrix1[0][0], DIM * DIM / proc, MPI_INT, MPI_IN_PLACE, DIM * DIM / proc, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = start; i < end; i++)
        for (int j = 0; j < DIM; j++)
        {
            res[i][j] = 0;
            for (int k = 0; k < DIM; k++)
                res[i][j] += matrix1[i][k] * matrix2[k][j];
        }

    MPI_Gather(MPI_IN_PLACE, DIM*DIM/proc, MPI_INT, &res[0][0], DIM*DIM/proc, MPI_INT, 0, MPI_COMM_WORLD);

        //this function is used to setup the device, context, queue program and kernel -- make sure to set file name (1) and kernel name (2)
    setup_openCL_device_context_queue_kernel( (char*) "./matrix_ops.cl" , (char*) "multiply_matrices");


   //this function is used to load/copy memory and link arguments -- you will need to change this 
   //for your program as this varies from one implementation to another
    setup_kernel_memory();
    copy_kernel_args();


    //submit the kernel for execution 
    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, local, 0, NULL, &event);
    clWaitForEvents(1, &event);


   //copying data from the device back to host c matrix
    clEnqueueReadBuffer(queue, bufC, CL_TRUE, 0, DIM * DIM *sizeof(int), &res[0][0], 0, NULL, NULL);

}

void node(int proc, int start, int end){

    MPI_Bcast(matrix2, DIM * DIM, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(&matrix1[0][0], DIM * DIM / proc, MPI_INT, &matrix1[start], DIM * DIM / proc, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = start; i < end; i++)
        for (int j = 0; j < DIM; j++)
        {
            res[i][j] = 0;
            for (int k = 0; k < DIM; k++)
                res[i][j] += matrix1[i][k] * matrix2[k][j];
        }

     MPI_Gather(&res[start], DIM*DIM/proc, MPI_INT, &res, DIM*DIM/proc, MPI_INT, 0, MPI_COMM_WORLD);

}

void create(int matrix[DIM][DIM])
{
  for (int i=0; i<DIM; i++)
    for (int j=0; j<DIM; j++)
      matrix[i][j] = rand() % MAX_RAND;
}

void output(int matrix[DIM][DIM])
{
  for (int i=0; i<DIM; i++) {
    for (int j=0; j<DIM; j++)
      std::cout<<matrix[i][j]<<"\t";
    std::cout<<"\n";
  }
    std::cout<<"\n";
}

void free_memory() {
   //free the buffers
   clReleaseMemObject(bufA);
   clReleaseMemObject(bufB);
   clReleaseMemObject(bufC);

    //free opencl objects
   clReleaseKernel(kernel);
   clReleaseCommandQueue(queue);
   clReleaseProgram(program);
   clReleaseContext(context);
   
}
void copy_kernel_args() {
    clSetKernelArg(kernel, 0, sizeof(int), (void*)&max);
    clSetKernelArg(kernel, 1, sizeof(int), (void*)&max);
    clSetKernelArg(kernel, 2, sizeof(int), (void*)&max);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&bufA);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&bufB);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&bufC);
    if(err < 0) {
      perror("Couldn't create a kernel argument");
      printf("error = %d", err);
      exit(1);
   }
}
void setup_kernel_memory() {
     bufA = clCreateBuffer(context, CL_MEM_READ_ONLY,  DIM*DIM*sizeof(int), NULL, NULL);
     bufB = clCreateBuffer(context, CL_MEM_READ_ONLY,  DIM*DIM*sizeof(int), NULL, NULL);
     bufC = clCreateBuffer(context, CL_MEM_READ_WRITE, DIM*DIM*sizeof(int), NULL, NULL);

    // Copy matrices to the GPU
    clEnqueueWriteBuffer(queue, bufA, CL_TRUE, 0, DIM*DIM*sizeof(int), &matrix1[0][0], 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufB, CL_TRUE, 0, DIM*DIM*sizeof(int), &matrix2[0][0], 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufC, CL_TRUE, 0, DIM*DIM*sizeof(int), &res[0][0], 0, NULL, NULL);

}

void setup_openCL_device_context_queue_kernel(char* filename, char* kernelname) {
    device_id = create_device();
    cl_int err;
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
   if(err < 0) {
      perror("Couldn't create a context");
      exit(1);   
    }

    program = build_program(context, device_id, filename );
    queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    if(err < 0) {
      perror("Couldn't create a command queue");
      exit(1);   
    };

    kernel = clCreateKernel(program, kernelname, &err);
    if(err < 0) {
      perror("Couldn't create a kernel");
      printf("error =%d", err);
      exit(1);
    };

}
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
  

   /* Read program file and place content into buffer */
   program_handle = fopen(filename, "r");
   if(program_handle == NULL) {
      perror("Couldn't find the program file");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   /* Create program from file 

   Creates a program from the source code in the add_numbers.cl file. 
   Specifically, the code reads the file's content into a char array 
   called program_buffer, and then calls clCreateProgramWithSource.
   */
   program = clCreateProgramWithSource(ctx, 1, 
      (const char**)&program_buffer, &program_size, &err);
   if(err < 0) {
      perror("Couldn't create the program");
      exit(1);
   }
   free(program_buffer);

   /* Build program 

   The fourth parameter accepts options that configure the compilation. 
   These are similar to the flags used by gcc. For example, you can 
   define a macro with the option -DMACRO=VALUE and turn off optimization 
   with -cl-opt-disable.
   */
   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if(err < 0) {

      /* Find size of log and print to std output */
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            0, NULL, &log_size);
      program_log = (char*) malloc(log_size + 1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            log_size + 1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }

   return program;
}

cl_device_id create_device() {

   cl_platform_id platform;
   cl_device_id dev;
   int err;

   /* Identify a platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   } 

   // Access a device
   // GPU
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      // CPU
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
   }
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);   
   }

   return dev;
}

/* References */
// SIT315 - lecture 8
// https://en.wikipedia.org/wiki/OpenCL
// https://cs.nyu.edu/courses/fall14/CSCI-GA.3033-004/lecture10.pdf
// https://www.eecg.utoronto.ca/~amza/ece1747h/homeworks/examples/MPI/other-examples/mmult.c
