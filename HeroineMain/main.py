import numpy as np
import cv2
#from matplotlib import pyplot as plt

cap = cv2.VideoCapture( 0 )

# Print ALL the variables, because it doesn't work otherwise
# No clue what most of these values mean
for i in range( 0, 18 ):
    print( cap.get( i ) )

# params for ShiTomasi corner detection
feature_params = dict( maxCorners = 200,
                       qualityLevel = 0.3,
                       minDistance = 15,
                       blockSize = 7 )

# Parameters for lucas kanade optical flow
lk_params = dict( winSize  = (15,15),
                  maxLevel = 2,
                  criteria = (cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 0.03))

# Create some random colors
color = np.random.randint(0,255,(100,3))

# Take first frame and find corners in it
ret, old_frame = cap.read()
old_gray = cv2.cvtColor(old_frame, cv2.COLOR_BGR2GRAY)
p0 = cv2.goodFeaturesToTrack(old_gray, mask = None, **feature_params)

# Create a mask image for drawing purposes
mask = np.zeros_like(old_frame)

while(1):
    ret, frame = cap.read()
    frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    
    # calculate optical flow
    p1, st, err = cv2.calcOpticalFlowPyrLK(old_gray, frame_gray, p0, None, **lk_params)

    # Select good points
    good_new = p1[st==1]
    good_old = p0[st==1]

    # draw the tracks
    for i,(new, old) in enumerate( zip( good_new, good_old ) ):
        a,b = new.ravel()
        c,d = old.ravel()
        mask = cv2.line(mask, (a,b),(c,d), color[i].tolist(), 2)
        frame = cv2.circle(frame,(a,b),5,color[i].tolist(),-1)
    img = cv2.add(frame,mask)

    # Now update the previous frame and previous points
    old_gray = frame_gray.copy()
    p0 = good_new.reshape(-1,1,2)

    # The depth map is normalized but a bit inverted. However, every depth value
    # less than 0.5 (or a tuned value) can be considered a way to go. For each of these pixels,
    # we can check the height on the image. If one of the pixels is below the given height,
    # it may represent a possible pathway. We can then run in a line to the right. If there
    # are enough pixels that way, great! We can go there.
    #     The tracks are also useful. We can track on just one camera. For each track, the
    # closest tracks with a similar (to be determined through tuning) depth value are considered
    # part of the asme object. For every object, the average movement of its tracks is the movement
    # of the object. The average depth of an object's tracks is the object's depth. The objects
    # are then sorted by depth. Heroine chooses the deepest pathway that is not in the path of
    # any moving object. If that pathway is within 10 degrees of Heroin'e current heading,
    # Heroine goes straight. Otherwise, Heroine turns by 30 degrees and continues.
    
##    depthTex = []   # This does something, not yet
##    depthMask = depthTex.copy()
##    for i in range( 0, len(depthTex) ):
##        if depthTex[i] > 0.03:
##            depthMask[i] = 1
##        else:
##            depthMask[i] = 0
##    # We should have the mask now of all the pixels we can go to
##    #
##    # Analyze the tracks, finding the ones with the closest depth values
    for i in range( 0, len( p0 ) ):
        for j in range( i, len( p0 ) ):
            if depth[p0[i].position] - depth[p0[j].position] < maxDepthDiff

    cv2.imshow( 'frame', img )
    k = cv2.waitKey( 30 ) & 0xff
    if k == 27:
        break
    
cv2.destroyAllWindows()
cap.release()
