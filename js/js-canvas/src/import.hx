#if js

import js.html.DOMElement;
import js.html.MouseEvent;
import js.html.CanvasElement;
import js.html.CanvasRenderingContext2D;

import js.html.KeyboardEvent;
import js.html.KeyboardEvent as K;

import Macros.text;

// canvas size
import Globals.APP_HEIGHT;
import Globals.APP_WIDTH;
// fps
import Globals.FPS_WANTED;
import Globals.FPS_INTERVAL;
// gravity
import Globals.GRAVITY_FORCE;
import Globals.PIXELS_PER_METER;
// const css
import Globals.CSS_BLOCK;
import Globals.CSS_NONE;
import Globals.CSS_EMPTY;

// global variable
import Globals.console;
import Globals.document;
import Globals.window;
import Globals.performance;

import Utils.lerp;
import Utils.toFixed;

#end
