import React, {useCallback, useMemo, useRef, useState} from 'react';
import {View, Text, StyleSheet, TextInput} from 'react-native';
import BottomSheet, {
  BottomSheetTextInput,
  BottomSheetView,
} from '@gorhom/bottom-sheet';

const App = () => {
  // ref
  const [name, setName] = useState('');

  const bottomSheetRef = useRef<BottomSheet>(null);

  // variables
  const snapPoints = useMemo(() => ['15%', '15%'], []);

  // callbacks
  const handleSheetChanges = useCallback((index: number) => {
    console.log('handleSheetChanges', index);
  }, []);

  // renders
  return (
    <View style={styles.container}>
      <BottomSheet
        ref={bottomSheetRef}
        index={1}
        snapPoints={snapPoints}
        keyboardBehavior={'interactive'}
        keyboardBlurBehavior={'restore'}
        handleComponent={null}
        // style={{backgroundColor: 'red'}}
        onChange={handleSheetChanges}>
        <BottomSheetView style={{backgroundColor: 'red', flex: 1}}>
          <TextInput placeholder="klkc" value={name} onChangeText={setName} />
          <View style={styles.contentContainer}>
            <Text>Awesome 🎉</Text>
          </View>
        </BottomSheetView>
      </BottomSheet>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    padding: 24,
    backgroundColor: 'grey',
  },
  contentContainer: {
    flex: 1,
    alignItems: 'center',
  },
  input: {
    marginTop: 8,
    marginBottom: 10,
    borderRadius: 10,
    fontSize: 16,
    lineHeight: 20,
    padding: 8,
    backgroundColor: 'rgba(151, 151, 151, 0.25)',
  },
});

export default App;
