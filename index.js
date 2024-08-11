/**
 * @format
 */

import {store} from '@library/store';
import {AppRegistry, View, Image, Text} from 'react-native';
import {Provider} from 'react-redux';
import App from './App';
import {name as appName} from './app.json';
import {GestureHandlerRootView} from 'react-native-gesture-handler';
import {NavigationContainer} from '@react-navigation/native';
import {SafeAreaProvider} from 'react-native-safe-area-context';
import {ToastProvider} from 'react-native-toast-notifications';
import {Images} from '@images/index';
import {Colors} from '@theme/colors';
import FastImage from 'react-native-fast-image';
import {navigationRef} from './app/navigation/rootNavigation';
import {MMKV} from 'react-native-mmkv';
export const storage = new MMKV();
const ReduxToolkitWrapper = () => (
  <Provider store={store}>
    <GestureHandlerRootView style={{flex: 1}}>
      <SafeAreaProvider>
        <NavigationContainer ref={navigationRef}>
          <ToastProvider
            placement="bottom"
            // Custom type example
            renderType={{
              custom_toast: toast => (
                <View
                  style={{
                    height: 300,
                    width: '100%',

                    justifyContent: 'flex-end',
                    alignItems: 'center',
                  }}>
                  <View
                    style={{
                      maxWidth: '90%',
                      paddingHorizontal: 15,
                      paddingVertical: 10,
                      backgroundColor: '#fff',
                      marginVertical: 4,
                      borderRadius: 8,
                      justifyContent: 'center',
                      paddingLeft: 16,
                      elevation: 10,
                    }}>
                    {/* <Image
                  source={require('../../assets/images/pop.gif')}
                  style={{height: 100, width: 250}}
                  // resizeMode="contain"
                /> */}
                    <View
                      style={{
                        flexDirection: 'row',
                        justifyContent: 'space-between',
                        width: '100%',
                        alignItems: 'center',
                      }}>
                      <Image
                        source={Images.tick}
                        style={{height: 20, width: 20}}
                        resizeMode="contain"
                      />
                      <Image
                        source={Images.cross}
                        style={{height: 15, width: 15}}
                        resizeMode="contain"
                      />
                    </View>
                    <View style={{rowGap: 5, marginVertical: 5}}>
                      <Text
                        style={{
                          fontSize: 14,
                          color: Colors.blp,
                          fontWeight: '600',
                          fontFamily: 'Inter-SemiBold',
                        }}>
                        Aromi Tray Paired Successfully!
                      </Text>
                      <Text
                        style={{
                          fontSize: 12,
                          color: '#344054',
                          fontWeight: '400',
                          fontFamily: 'Inter-Regular',
                        }}>
                        Your Aromi Tray is now connected! You can add your spice
                        jars and start organizing your spice collection.
                      </Text>
                    </View>
                  </View>

                  <FastImage
                    resizeMode="contain"
                    source={Images.pop}
                    style={{
                      height: '70%',
                      width: '100%',
                      position: 'absolute',
                      zIndex: 999,
                      bottom: 20,
                    }}
                  />
                </View>
              ),
            }}>
            <App />
          </ToastProvider>
        </NavigationContainer>
      </SafeAreaProvider>
    </GestureHandlerRootView>
  </Provider>
);

AppRegistry.registerComponent(appName, () => ReduxToolkitWrapper);
