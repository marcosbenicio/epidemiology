#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include<math.h>


#define N       1000    //total population
#define time    1000    //simulation time
#define sample  10      //number of repeated samples

#define lambda  0.3     //infection probability (order parameter)

#define So      0.95     //initial proportion of susceptible individuals

int main()
{

   int state[N], state_sync[N]; // arrays to hold the state of each individual
   // arrays to store the number of individuals in each state
   double S[time], I[time];    

   int I_count, S_count; // counters for the number of infected and susceptible individuals

   double S_ave, I_ave; // variables to hold the average number of susceptible and infected individuals over time

   int i, j, s, t; // loop variables

   int r;             // random number generator seed
   double rand;       // variable to hold random numbers generated by the seed
   unsigned int m, a; // parameters for the random number generator

   m = 2147483647; // maximum value of the random number generator
   a = 16807;      // multiplier for the random number generator
   r = 27;         // initial seed value for the random number generator

   char arquivo[200];
   FILE *fp;

   sprintf(arquivo, "N%i_time%i_S%.2lf_lambda%.2lf_alpha%.2lf.txt", N, time, So, lambda, alpha);
   fp = fopen(arquivo, "w");

   fprintf(fp, "#time\t\t\t\t#S\t\t\t\t#I\n");

   for (t = 0; t < time; t++)
   {
      S[t] = 0.0;
      I[t] = 0.0;
   }

   for (s = 1; s <= sample; s++)
   { // loop over samples
      S_count = 0;
      I_count = 0;

      for (i = 0; i < N; i++)
      { // initialize the state of each individual in the population

         r = (r * a) % m;
         rand = (double)r / m;
         // if the random number is less than the initial proportion of susceptible individuals, set the individual to be susceptible
         if (rand < So)
         {
            state[i] = 0;
            state_sync[i] = 0;
            S_count++;
         }
         else
         { // otherwise, set the individual to be infected
            state[i] = -1;
            state_sync[i] = -1;
            I_count++;
         }
      }
      // add the initial number of susceptible and infected individuals to the S and I arrays
      S[0] += (double)S_count;
      I[0] += (double)I_count;

      for (t = 1; t < time; t++)
      { // loop over the simulation time

         // ################################################
            /************  Epidemic dynamics *************/
         // ################################################
         //  This loop runs the epidemic simulation for each individual in the population
         for (i = 0; i < N; i++)
         {

            // If an individual is healthy (state = 0), they may become infected based on the status of another random individual (j)
            if (state[i] == 0)
            {

               // Generate a random number
               r = (r * a) % m;
               // Map the random number to an integer between 0 and N-1 to select a random individual
               j = N * ((double)r / m);

               // Ensure that the randomly selected individual is not the same as the current individual (i)
               // If j is equal to i, repeat the random selection process until j != i
               do
               {
                  r = (r * a) % m;
                  j = N * ((double)r / m);
               } while (j == i);

               // If the randomly selected individual is infected (state[j] = -1),
               // there is a chance that the healthy individual (i) becomes infected too
               if (state[j] == -1)
               {

                  // Generate another random number to determine if the healthy individual (i) becomes infected
                  r = (r * a) % m;
                  rand = (double)r / m;

                  // If the randomly generated number is less than or equal to the transmission probability (lambda),
                  // the healthy individual (i) becomes infected
                  if (rand <= lambda)
                  {
                     state_sync[i] = -1;
                  }
               }
            }
         }

         // Update the state of each individual synchronously
         for (i = 0; i < N; i++)
            state[i] = state_sync[i];

         // Count the number of healthy (S_count) and infected (I_count) individuals in the population
         S_count = 0;
         I_count = 0;
         for (i = 0; i < N; i++)
         {
            if (state[i] == 0)
               S_count++;
            if (state[i] == -1)
               I_count++;
         }

         // Add the number of healthy (S_count) and infected (I_count) individuals to the corresponding arrays (S and I)
         S[t] += (double)S_count;
         I[t] += (double)I_count;
      } // finish time loop
   }    // finish samples loop

   // Calculate the average number of healthy and infected individuals for each time step across all simulation runs
   for (t = 0; t < time; t++)
   {
      S_ave = (double)S[t] / (N * sample);
      I_ave = (double)I[t] / (N * sample);

      // Write the average number of healthy and infected individuals to a file
      fprintf(fp, "%i\t\t\t\t%lf\t\t\t\t%lf\n", t, S_ave, I_ave);
   }
   // Close the output file
   fclose(fp);
}
