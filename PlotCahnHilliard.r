# First setup of the model
#remove(list=ls()) # Remove all variables from memory

on=1;off=0;
setwd('/Simulations/OpenCL/clCahnHilliard/clCahnHilliard')

require(fields)

FID = file("clCahnHilliard.dat", "rb")

A=readBin(FID, integer(), n = 3, endian = "little");

Movie=on
Wait=off
WinWidth = 960
WinHeight = 720

NX= A[1]
NY = A[2]
NumFrames = A[3]

RecordTimes = readBin(FID, numeric(), size=8, n = NumFrames+1, endian = "little");

DPI=144

#ColorPalette = topo.colors
ColorPalette = colorRampPalette(c("#00007F", "blue", "#007FFF", "cyan",
                                  "#7FFF7F", "yellow", "#FF7F00", "red", "#7F0000"))

if (Movie==off) 
  quartz(width=WinWidth/DPI, height=WinHeight/DPI, dpi=DPI)

for (jj in 0:(NumFrames-1)){  # Here the time loop starts 
  
   if (Movie==on)
      jpeg(filename = sprintf("Images/Rplot%03d.jpeg",jj),
           width = WinWidth, height = WinHeight, 
           units = "px", pointsize = 24,
           quality = 100,
           bg = "white", res = NA,
           type = "quartz")  
   
   Data_C = matrix(nrow=NY, ncol=NX, readBin(FID, numeric(), size=4, n = NX*NY, endian = "little"));
   
   par(mar=c(1, 4, 1, 6) + 0.1)
   
   image.plot(Data_C, zlim=c(-1.1,1.1), xaxt="n", yaxt="n",
              col = ColorPalette(255),asp=1, bty="n", useRaster=TRUE,
              legend.shrink = 0.85, legend.width = 2)  
   
   title("The Cahn-Hilliard model", line=-0.5)   
   
   mtext(text=paste("Time : ",sprintf("%1.0f",RecordTimes[jj+1]),
                    "of" ,sprintf("%1.0f",RecordTimes[NumFrames]), "Time units"), 
                    side=1, adj=0.5, line=-0.5, cex=1)
   
   if (Movie==on) dev.off() else { 
     dev.flush()
     Sys.sleep(0.01)
     dev.hold()
   }
   if (Wait==on){
     cat ("Press [enter] to continue, [q] to quit")
     line <- readline()
     if (line=='q'){ stop() }
   } 
}

close(FID)

if (Movie==on) { 
  
   InFiles=paste(getwd(),"/Images/Rplot%03d.jpeg", sep="")
   OutFile="CahnHilliard.mp4"
  
   print(paste(" building :", OutFile))
  
   CmdLine=sprintf("ffmpeg -y -r 30 -i %s -c:v libx264 -pix_fmt yuv420p -b:v 10000k %s", InFiles, OutFile)
   cmd = system(CmdLine)
     
} 

system('say All ready')