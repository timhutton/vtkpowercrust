// .NAME vtkPowerCrustSurfaceReconstruction - reconstructs surfaces from unorganized point data
// .SECTION Description
// vtkPowerCrustSurfaceReconstruction reconstructs a surface from unorganized points
// scattered across its surface. The original algorithm is the Power Crust, for full details and
// for the original code, see:
//
// http://www.cs.utexas.edu/users/amenta/powercrust/welcome.html
//
// IMPORTANT: The PowerCrust code was released under the GNU public licence (GPL) - this forbids
// its use for developing commercial products! As a modified version, this port therefore has the
// same restrictions. For more details see the copyrights in the source file, and:
//
// http://www.gnu.org/copyleft/gpl.html
//
// -- The restriction applies only to this class. --
//
// The medial surface can be accessed using GetMedialSurface() - remember to call Update() on the
// filter before accessing this, it is not part of the normal VTK pipeline.
//
// This filter is a big improvement on vtkSurfaceReconstructionFilter in almost all cases but it is
// not as fast.
//
// .SECTION Thanks
// This VTK port was created by: Arash Akbarinia, Tim Hutton, Bruce Lamond, Dieter Pfeffer, Oliver
// Moss.
//
// .SECTION Caveats
// The algorithm may fail to give a correct reconstruction on surfaces that are not densely
// sampled. In practice it does very well.
//
// The exact arithmetic routines are thought to have problems on some platforms, please report any
// problems you encounter.
//
// The orientation of the polygons is not consistent! This can be corrected by
// vtkPolyDataNormals (ConsistencyOn) but you should be aware of it.
//
// The surface has not been simplified using the routines provided with the distribution, this will
// hopefully come soon.
//
// .SECTION Example
// Please check the example.cxx file.

/* -----------------------------------------------------------------

  Cleaned up the code and resolved memory leaks.

  Arash Akbarinia 26/05/2014
  akbarinia.arash_AT_gmail.com

  Summary of chanegs:
    - enclosing everything in one class. This helps the memory management and let's you run this
       algorithm inside another application, e.g. Slicer. With the original code it crashes due
       to global variables.
    - resolving all the memory leaks.
    - removing the code which was never used, it reduced the code from 12000 to 4500.
    - setting the mult_up variable to the number of points in the point cloud multilply by 10

  Of course there is still much more that can be done. Hopefully the current version is easier to
  read and can attract other contributors. No of variables or functions have been renames, this
  can help future contributors to compare this version with the older ones.

  TODO:
    - the construced surface sometimes contains holes, this must be investigated and resolved.
    - work out some scheme for getting rid of mult_up, based on precision of input points.
    - include simplify and orient to make good-looking output.

------------------------------------------------------------------- */

/* -----------------------------------------------------------------

  "Porting PowerCrust to VTK"
     or
  "The Things We Do for Fun These Days"

  Tim J. Hutton 28/06/2002
  T.Hutton@eastman.ucl.ac.uk

  Some history:
  Nina Amenta et al. came up with a lovely algorithm for surface
  reconstruction - the PowerCrust. With humbling generosity they made
  their code available to the world, under the GNU Public License. 
  To make it easier to use for everyone, I (with much encouragement 
  and help from many others) decided to port it to VTK. This is that story.

  The original code can be found at:

  http://www.cs.utexas.edu/users/amenta/powercrust/welcome.html

  Getting it to compile and run on my Windows box was fun. Notes on how to 
  do this can be found at the bottom of this file.

  Getting it to compile under VTK was a battle and a half. The word 'beast'
  doesn't begin to describe it. A lot of little changes were necessary, many
  of them concerned with getting rid of the use of files as temporary
  storage. More details are at the bottom of this file.

  The powercrust code includes robust routines for computing voronoi triangularizations. 
  I wanted to make this available for other routines in VTK but was beaten back 
  by the thicket. It would take a better person than I to make this useful elsewhere. 
  It is a sign of defeat that I've munged the source files into one - it does makes 
  it neater from a VTK point of view since there is only one file.

------------------------------------------------------------------- */

/* -----------------------------------------------------------------

  Modification of Tim's VTK port

  by Dieter Pfeffer   d.pfeffer@eprosoft.de
  and Oliver Moss   o.moss@eprosoft.de

  10/06/03

  all changed sourcecode is mentioned with EPRO

  modifications:

  - to change est_r - because this value has a great influence on the output
    GetEstimate_r ()
    SetEstimate_r (double)

  - to implement an init function to reuse an instance of the filter for doing surface reconstruction again
    void pcInit ();  
    it's running but not all global or static variables are reinitialized - some functional 
    static variables are set global to reinitialize these variables

  still Memory leaks

--------------------------------------------------------------------- */

/*=================TimsFun.txt===============================================================

Today I will try to compile the PowerCrust code on my Win2K box.
One day I dream of PowerCrust being a self-contained VTK class, how happy we'd be.

Phase One - Compiling PowerCrust on a Windows box
-------------------------------------------------

Downloaded the source from Nina: powercrust.tar.gz
from the PowerCrust homepage: http://www.cs.utexas.edu/users/amenta/powercrust/welcome.html
Looked in the Makefile for the files needed to compile the main powercrust application.
Built a VC++7 project containing:

crust.c
fg.c
heap.c
hull.c
hullmain.c
io.c
label.c
math.c
pointops.c
power.c
predicates.c
rand.c
ch.c

plus:

stormacs.h
hull.h
points.h
pointsites.h

Hit F7 to build the application. Ah, the naivete!
Of couse there was an endless stream of error messages.

\powercrust\predicates.c(119) : fatal error C1083: Cannot open include file: 'sys/time.h': 
No such file or directory

So I commented out the #include line of predicates.c, and the error went away. (if only they 
were all this easy)

\powercrust\hullmain.c(44) : fatal error C1083: Cannot open include file: 'getopt.h': No such 
file or directory

So I found the files getopt.h and getopt.c somewhere (c:\texmf\doc\graphics\texdraw - part of 
my LaTeX distrib!) and added 

them to the project. Man this is crazy. Guess Unix people get these files with the system.

Lots of errors to do with old-style C function declarations - we can set a flag in the compiler 
to cope with this. In VC++7 it was a matter of removing the /clr on the property pages : 
Configuration Properties : C/C++ : General : Compile As Managed = Not using managed extensions.

It compiles! Glory be.

Lots of link errors though (did you think it would be that easy?). Some saying random() was 
not defined. Hmmm... Surely rand() does what we want. Likewise for srandom().

Created a new file tim_defs.h with the lines:

#include <stdio.h>
int random() { return rand(); }
void srandom(int s) { srand(s); }

And #included it in hullmain.c. Seemed to do the trick.

Next link error:

io.obj : error LNK2019: unresolved external symbol _popen referenced in function _epopen

Hmmm.. what is popen? A net search reveals that some people think it is part of stdio.h - but 
stdio.h is #included in io.c, so something is wrong. Must be another Unix v. Windows issue.

Further search revealed this page:

http://developer.gnome.org/doc/API/glib/glib-windows-compatability-functions.html

Which is all very interesting. I try to copy the port for popen and pclose by adding the lines:

#define popen _popen
#define pclose _pclose

to the file tim_defs.h. This seems to work. If only I knew what I was doing.

Next link error:

rand.obj : error LNK2019: unresolved external symbol _erand48 referenced in function _double_rand

Another function in the Unix libraries it seems, stdlib.h this time. Hmmm...

Eventually found this page:

http://www.ics.uci.edu/~eppstein/projects/pairs/Source/testbed/rand48/

(David Eppstein is one of the guys who worked with Nina, but google didn't know that...)

So I borrowed the files:

_rand48.c
drand48.c
erand48.c
nrand48.c
rand48.3
rand48.h
seed48.h
seed48.c
srand48.c

and added them to the project... and do you know it actually worked! 

Final link error said logb wasn't defined. Looked in rand.c, the code there looked funny, 
so I did this instead:

// commented out by TJH (how many people do you know that work on a cray?)

//#ifdef cray
//double logb(double x) {
//  if (x<=0) return -1e2460;
//  return log(x)/log(2);
//}
//#endif

double logb(double x)
{
  return log(x)/log(2);
}

A fool marches on where wish men fear to tread.

And.. and.. and.. it's linked. powercrust.exe has been produced. You can run it and everything. 
It speaks:

reading from stdin
main output to stdout

If you say (like in the README)

powercrust -m 10000000 -i hotdogs.pts

then it starts doing its stuff, churning away producing data. Some of it goes wrong for various 
reasons (like 'cat' not being a command in windows...)

It produces a whole series of files:

axis
axisface
pole
sp
pc
pnf
axis.off
axisface.off
head
inpball
inpole
outpole
pc.off
poleinfo
tpoleinfo
di
ma
re
rere

<sigh>

Some of these are described in the README - eg. pc.off will contain the final surface. Sadly 
all the .off files are of zero size, because the program didn't run to completion.

In hullmain.c, in the main() function there are the system calls to 'cat'. If we change these 
to 'type' then the DOS command works instead. Likewise we can change 'rm' to 'del'.

Now the program works, only the files mentioned in the README are left over. We can load the 
file pc.off into our favourite viewer and view the surface created. (Should there be a vtkOFFReader?)

So, now all we need to do is modify its file processing to use VTK data structures and we're 
done. vtkPowerCrust here we come!


Phase Two - Porting PowerCrust to VTK
-------------------------------------

We'd like to be able to release PowerCrust so all the VTK users can use it. This means compiling 
it as a class to be released with VTK. This means no system-dependent calls (like 'type' and 'del') 
and ideally no intermediate files saved to disk. Also, ideally all the code would be in one file 
rather than spread over twenty. Too much to ask? We will see.

The first and most obvious thing is to produce a wrapper, a skeleton vtkDataSetToPolyDataFilter 
derivative that calls the powercrust code as is and uses the output. Then gradually we squeeze 
everything into the wrapper.

I used #include "hullmain.c" and lots of similar lines to include the .c files in the file I was
creating, vtkPowerCrustSurfaceReconstruction.cxx. Yes, I know this looks awful but the final goal 
was to, yes, spooge everything into one file for VTK neatness. Got lots of errors, names colliding, 
old style function declarations being choked on, etc.

Working through the errors one by one. Lots of little changes all over, I'm afraid - all marked 
with 'TJH'. Had to be done (or so it seems to me at the moment). The most tedious bit so far was 
replacing the function declarations in predicates.c... The good news is that it compiles and links 
again. 

So, all we've got to do now is make it work in VTK, hook the functionality up so that the input 
vtkPointSet is passed as a set of points to the PowerCrust code, and the output surface is returned 
as a vtkPolyData. At the moment the powercrust code uses lots of temporary files for storage, this 
is really strange to my eyes, maybe it saves on memory but surely at the expense of a lot of speed.

Commented out the main() function in hullmain.c, moved it over to vtkPowerCrustSurfaceReconstruction.cxx 
so we can do evil things with it and bend it to our own ends. Basically our first-pass approach is 
to replace file reading with taking data from a vtkDataSet, leaving everything else as is. Managed 
to lose getopt.c and getopt.h which is nice because they came from elsewhere, did this by commenting 
out the big switch statement that parsed the command-line options.

Made an adapted version of read_next_site() called vtk_read_next_site() that takes data from our 
input vtkDataSet instead of from INFILE. We still need the old version because the file 'sp' is read 
in again (as INFILE) after being written to (as SPFILE). (sigh)

Man the code is hairy, loads of globals and stuff. I mean, lots of respect to all the authors and 
that for a) the algorithms they came up with and b) making their code available but really, 
this is like code from the eighties. (maybe it is...)

Hopefully soon we should be able to use the filter to take a bunch of points and produce its pc.off 
output - which we can check. If this works then we can go ahead and pipe the output not to file but 
to a vtkPolyData. 

Well, it kind of worked, got some output. Some warnings about unlabelled poles which are probably 
because the cactus data has a hole in it. Will try hooking up the output directly. Ok, done that. 
Had to read in the final file instead of taking the data from the point at which it was produced, 
which is odd. But it works now, can use powercrust in a tcl script. 

Sometimes hangs. Memory leaks also. Oh, and have to keep deleting the intermediate output files 
else get all sorts of problems - this file stuff is a real mess and the biggest problem with the 
powercrust code.

Solved the hanging - there was a conversion from char* to double I'd managed to overlook...
(indface[i])

OK, managed to get rid of all of the file handling. In the end it wasn't too hard - the temporary 
file was never actually being read in for example, since the function off_out doesn't get used. 
The file 'sp' contained the poles and their weights, which we now store as vtk_medial_surface and 
its scalars.

Got the code all into one file for neatness in VTK. It looks awful now but I suspect there will not
be a lot of people coming in to clear things up, a reimplementation would probably be quicker.

One twist to the tale, to get good-looking surfaces with normals we need to run 'orient', that
came with the powercrust distrib as setNormals.C and ndefs.h. We will have to now include these in
code above somehow. Oh, and running 'simplify' results in a *much* simpler surface, so we should do
this too, this is more important. BTW, compiling these in windows isn't too hard, you need to start 
from a 'Win32 console application' in MSVC and deal with the errors that come up.

On the command line, the whole run might consist of:

powercrust -m 1000000 -i hotdogs.pts
simplify -i poleinfo -o simp_poles -n 1.0 -r 0.3
powercrust -p -i simp_poles

(with the final output in pc.off)

But we'd just commented out the code that dealt with the -p option, and outputting poleinfo, damn.


To-do list:
- get rid of the code we don't use (expecting a reasonable amount)
- work out some scheme for getting rid of mult_up, based on precision of input points.
- include simplify and orient to make good-looking output

=============================================================================================*/
