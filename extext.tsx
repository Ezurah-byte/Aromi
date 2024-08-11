import React from 'react';
import {StyleSheet} from 'react-native';
import Animated, {
  useSharedValue,
  useAnimatedStyle,
} from 'react-native-reanimated';
import {
  GestureHandlerRootView,
  Gesture,
  GestureDetector,
} from 'react-native-gesture-handler';

const MIN_SIZE = 100; // Minimum size for width and height
const INITIAL_TEXT_SIZE = 16; // Initial font size

const ExpandableBox = ({parentWidth, parentHeight}) => {
  const translateX = useSharedValue(0);
  const translateY = useSharedValue(0);
  const scale = useSharedValue(1);
  const initialScale = useSharedValue(1);
  const initialX = useSharedValue(0);
  const initialY = useSharedValue(0);
  const scaleOffsetX = useSharedValue(0);
  const scaleOffsetY = useSharedValue(0);

  // Pan gesture for dragging
  const panGesture = Gesture.Pan()
    .onBegin(event => {
      initialX.value = translateX.value;
      initialY.value = translateY.value;
    })
    .onUpdate(event => {
      const newX = initialX.value + event.translationX;
      const newY = initialY.value + event.translationY;

      // Calculate box dimensions with current scale
      const boxWidth = 100 * scale.value;
      const boxHeight = 100 * scale.value;

      // Constrain the box within the parent view
      const clampedX = Math.max(0, Math.min(newX, parentWidth - boxWidth));
      const clampedY = Math.max(0, Math.min(newY, parentHeight - boxHeight));

      translateX.value = clampedX;
      translateY.value = clampedY;
    });

  // Pinch gesture for scaling
  const pinchGesture = Gesture.Pinch()
    .onBegin(event => {
      scaleOffsetX.value = event.focalX - translateX.value;
      scaleOffsetY.value = event.focalY - translateY.value;
      initialScale.value = scale.value;
    })
    .onUpdate(event => {
      const newScale = event.scale * initialScale.value;
      const minScale = MIN_SIZE / 100;
      scale.value = Math.max(newScale, minScale);

      // Calculate box dimensions with new scale
      const boxWidth = 100 * scale.value;
      const boxHeight = 100 * scale.value;

      // Constrain height within the parent view
      const clampedHeight = Math.min(boxHeight, parentHeight);
      const clampedWidth = Math.max(boxWidth, MIN_SIZE);

      // Adjust translation to keep the box within the parent view
      translateX.value = Math.max(
        Math.min(
          translateX.value + (boxWidth - clampedWidth) / 2,
          parentWidth - clampedWidth,
        ),
        0,
      );
      translateY.value = Math.max(
        Math.min(
          translateY.value + (boxHeight - clampedHeight) / 2,
          parentHeight - clampedHeight,
        ),
        0,
      );

      // Ensure the box width can expand if space allows
      scale.value = Math.min(scale.value, parentWidth / 100);
    });

  // Combine pan and pinch gestures
  const combinedGesture = Gesture.Simultaneous(panGesture, pinchGesture);

  // Animated style for box
  const animatedStyle = useAnimatedStyle(() => {
    const size = 100 * scale.value;
    const clampedSize = Math.max(size, MIN_SIZE);
    return {
      width: clampedSize,
      height: Math.min(clampedSize, parentHeight),
      transform: [
        {translateX: translateX.value},
        {translateY: translateY.value},
      ],
    };
  }, [translateX.value, translateY.value, scale.value]);

  // Animated style for text
  const textStyle = useAnimatedStyle(() => {
    const fontSize = INITIAL_TEXT_SIZE * scale.value;
    return {
      fontSize,
    };
  }, [scale.value]);

  return (
    <GestureHandlerRootView style={styles.container}>
      <GestureDetector gesture={combinedGesture}>
        <Animated.View style={[styles.box, animatedStyle]}>
          <Animated.Text style={[styles.text, textStyle]}>
            Resize Me
          </Animated.Text>
        </Animated.View>
      </GestureDetector>
    </GestureHandlerRootView>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    // justifyContent: 'center',
    // alignItems: 'center',
  },
  box: {
    backgroundColor: 'skyblue',
    justifyContent: 'center',
    alignItems: 'center',
  },
  text: {
    color: 'white',
  },
});

export default ExpandableBox;
