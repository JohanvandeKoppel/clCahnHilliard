# The Cahn-Hilliard model for phase separation
# http://en.wikipedia.org/wiki/Cahn%E2%80%93Hilliard_equation

# First setup of the model
remove(list=ls()) # Remove all variables from memory

require("fields") # An R package to load

D = 1;
gamma = 0.5 
  
# Simulation parameters
Length=128             # 100  - The length of the simulated landscape, in meters
m=256                  # 100  - # gridcellen
dT=0.0025              # 0.1  - timestep
EndTime=400            #      - Time at which the simulation ends
Numframes=400          #      - Number of frames displayed during the entire simulation

WinWidth=7             #      - Width of the simulation window 
WinHeight=5            #      - Height of the simulation window

NX=NY=m
dx=Length/NX
dy=Length/NY

# ------ Function definitions
Laplacian <- function (C, dx, dy) {
  
  NX=dim(C)[2]
  NY=dim(C)[1]
  
  dC=matrix(nrow=NY, ncol=NX, data=NaN)
  
  dC[2:(NY-1),2:(NX-1)] = 
    (C[2:(NY-1),1:(NX-2)] + C[2:(NY-1),3:NX] - 2*C[2:(NY-1),2:(NX-1)])/dx/dx +
    (C[1:(NY-2),2:(NX-1)] + C[3:NY,2:(NX-1)] - 2*C[2:(NY-1),2:(NX-1)])/dy/dy
  
  return(dC)
  
}

# Initialisation: declaring variable and setting up initial values
# All arrays of dimension m x m

C = matrix(nrow=NY,ncol=NX, data=runif(NY*NX)*2-1)
dC = Intermediate = matrix(nrow=NY,ncol=NX, data=0)

RecordTimes=(0:Numframes)*EndTime/Numframes
Step=EndTime^(1/(Numframes-1))
RecordTimes=Step^(0:(Numframes-1))

AllData=array(data=0,dim=c(m,m,length(RecordTimes)))

Time  =  1        ; # Begin time 
jj    =  1        ; # Starting the plot counter with a high value

# ------- Setting up the figure ------------------------------------------
  
## Open a graphics window (Darwin stands for a Mac computer)
if (Sys.info()["sysname"]=="Darwin"){
    quartz(width=WinWidth, height=WinHeight, title="The Cahn-Hilliard model")
  } else
    win.graph(width = WinWidth, height = WinHeight)

par(mar=c(3, 4, 2, 6) + 0.1)

# ------------ The simulation loop ---------------------------------------

print(system.time(
while (Time<=EndTime){  # Here the time loop starts 
  
    Intermediate = C^3 - C - gamma * Laplacian(C,dx,dy)
    
    dC = D*Laplacian(Intermediate,dx,dy)
    
    C = C + dC*dT;
    
    # Circular boundary conditions
    
    C[,1:2]=C[,(NX-3):(NX-2)]
    C[1:2,]=C[(NY-3):(NY-2),]
    C[,(NX-1):NX]=C[,3:4]
    C[(NY-1):NY,]=C[3:4,]
    
    # Graphic representation of the model every now and then
    if ((Time+dT/100)>=RecordTimes[jj]){   # The dT/100 is to avoid inaccuracy problems
      
        image.plot(C, zlim=c(-1.1,1.1), xaxt="n", yaxt="n",
                   col = topo.colors(255),asp=1, bty="n",
                   legend.shrink = 0.99, legend.width = 2)  
        title("The Cahn-Hilliard model")   
     
        mtext(text=paste("Time : ",sprintf("%1.0f",Time),
                         "of" ,sprintf("%1.0f",EndTime), "timesteps"), 
                         side=1, adj=0.5, line=1.5, cex=1)
     
        dev.flush()
        dev.hold()
     
        AllData[,,jj]=C
     
        jj=jj+1 # Increasing the Recorder counter
     
    } 
    
    Time=Time+dT;  # Incrementing time by dT
    
  }
))

save(file='Cahn-Hilliard.rda',AllData,RecordTimes)

system('say Finished')