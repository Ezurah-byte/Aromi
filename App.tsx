import {Button, OtpField, BottomSheetView} from '@atoms/index';
import Example from './app/example/example';
import React, {useState} from 'react';
import {SafeAreaView, StatusBar, View, Text} from 'react-native';
import {PhoneNumber, TextField} from '@molecules/index';
import {Colors} from '@theme/colors';
import {sizeConfig} from '@utils/sizeConfig';
import StackMenu from './app/navigation/stackMenu';
import {useSafeAreaInsets} from 'react-native-safe-area-context';
import Sheettest from './sheetTest';
function App(): React.JSX.Element {
  const [number, setCurrentNumber] = useState(null);
  const [name, setName] = useState('');
  const insets = useSafeAreaInsets();
  return (
    <View
      style={{
        flex: 1,
        backgroundColor: 'white',
      }}>
      <StatusBar
        translucent
        barStyle={'dark-content'}
        backgroundColor={'transparent'}
      />
      {/* <Sheettest /> */}
      <StackMenu />
      {/* <Example /> */}
      {/* <PhoneNumber
        {...{
          currentValue: number,
          handleChange: setCurrentNumber,
          placeholder: 'EX:9834575932',
          holderColor: Colors.gy,
        }}
      />
      <TextField
        {...{
          currentValue: name,
          handleChange: setName,
          placeholder: 'Enter company name',
          holderColor: Colors.gy,
          inputStyle: {
            marginLeft: sizeConfig.ms(7),
          },
        }}
      />
      <Button />

      <OtpField />

      <BottomSheetView /> */}
    </View>
  );
}

export default App;
