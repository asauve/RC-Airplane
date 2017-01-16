# RC-Airplane
MATLAB code was used to select the best flying wing configuration through inviscid flow simulations, lifting line theory, and a nelder-mead optimization. C++ and arduino code allows the airplane to be radio controlled from a personal computer with an xbox controller plugged in. XBee tranceivers were used to transmit data between the laptop and the arduino. 

AERODYNAMICS EXPLAINED

A flying wing design was chosen because it looks sweet and has really low drag. the downside is a smaller flight envelope as the airplane will be less stable outside of the designed flight speeds and/or has increased drag outside of designed speeds. Flying wings do not have a fuselage, so there isnt a stabilizer on the tail. Due to this the wing tips must be twisted down, and the entire wing has to be swept back (flying V) for stability. A certain amount of twist must be chosen based on payload weight and flight speed: at other flight conditions the geometric twist of the wings from tip to root may be insufficient for stable flight or may be overly stable and create excess drag. At high speeds the twisted wing tips may even produce lift in the wrong direction (eg: down). the amount of sweep and twist required for stable flight was calculated using Dr. Walter Panknin's excel sheet (ported into the matlab code).


MATLAB CODE EXPLAINED

running the AlexMain.m file will begin a search for the lowest drag flying wing, given a certain payload weight. results of the simulation will be stored in results.txt, overwriting any previous entry.
The code uses the Tornado engine, a vortex panel inviscid flow simulator. the Tornado engine was modified from a text based Q&A session so that it could be used to batch simulate wings with varying parameters. The tornado engine returns a value for lift-induced drag, whereas  "experimental" results from XFOIL are used to map angle of attack to a profile drag coefficient. With restriction on sweep angle and geometric twist, the code searches for the lowest drag configuration which will satisfy stability conditions. airplane drag is dependant on 5 variables (span, root chord length, taper ratio, sweep angle, speed) (dihedral is held constant at 5 degrees); a nelder- mead optimization was used to iterate towards the lowest drag design. 
at the end, a speed-drag graph is plotted for the final design.
additional restrictions were placed on span and root chord lengths (too long of a span is impractical/wont fit in a trunk and too small of a root chord will not be structural and is harder to manufacture). these restrictions are hardcoded within Alex_modified_tornado.m: impractical wing configurations have the inviscid drag overwritten by a much larger amount. useful for debugging: if inviscid drag is 99N, we know that the wing was rejected based on root chord and/or span lengths.

C++ AND ARDUINO CODE EXPLAINED
C++ code only works on windows. searches for a connected xbox controller, then proceeds to read analog stick and button info and converts to corresponding flap settings. displays results in an active winforms application which uses progress bars to visually indicate the position of the flaps and throttle. sends the flap information to the serial port with XBee tranceiver.
arduino receives packets and sends information to the motor ESC and flap servos.
