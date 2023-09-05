// index.js
// 获取应用实例

const app = getApp()
const { connect } = require('../../utils/mqtt.min')
const mqttHost = 'www.eisbre.xyz'
const mqttPort = 8084

const devSubTopics = 'my/smarthome/sub'
const devPubTopics = 'my/smarthome/pub'

const mySubTopics = devPubTopics
const myPubTopics = devSubTopics

Page({
  data: {
    client: null,
    Temp: 0,
    Hum: 0,
    Light: 0,
    Led: false,
  },

  onLedChange(event){
    const that = this
    console.log(event.detail.value);
    that.setData({Led:event.detail.value})

    if(event.detail.value){
      that.data.client.publish(myPubTopics, JSON.stringify({
        "LED": 1
      }), function(err){
        if(!err){
          console.log('成功开灯');
        }
      })
    }
    else{
      that.data.client.publish(myPubTopics, JSON.stringify({
        "LED": 0
      }), function(err){
        if(!err){
          console.log('成功关灯');
        }
      })
    }
  },

  onShow() {
    const that = this
    that.setData({
      //www.eisbre.xyz
      // client: connect('wxs://broker-cn.emqx.io:8084/mqtt')
      client: connect(`wxs://${mqttHost}:${mqttPort}/mqtt`)
    })

    that.data.client.on('connect', function(){
      console.log("连接mqtt服务器成功")
      wx.showToast({
        title: '连接成功',
        icon: 'success',
        mask: true
      })

      that.data.client.subscribe(mySubTopics, function(err){
        if(!err){
          console.log('成功订阅上行Topics');
        }
      })

      that.data.client.on('message', function(topic, message){
        //console.log(message);
        //message是16进制buffer字节流
        let dataFromDev = {}
        try {
          dataFromDev = JSON.parse(message)
          console.log(dataFromDev);
          that.setData({
            Temp: dataFromDev.Temp,
            Hum: dataFromDev.Hum,
            Light: dataFromDev.Light,
            Led: dataFromDev.LED,
          })
        } catch (error) {
          console.log("JSON解析失败");
        }
      })
    })
  },
});

