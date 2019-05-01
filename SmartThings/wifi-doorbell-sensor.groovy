/**
 *  WiFi Doorbell Sensor - Device Handler for SmartThings
 *
 *  Copyright 2019 Firefly Electronix
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 *  in compliance with the License. You may obtain a copy of the License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 *  on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License
 *  for the specific language governing permissions and limitations under the License.
 */

import groovy.json.JsonSlurper

metadata {
	definition (name: "WiFi Doorbell Sensor", namespace: "fireflyelectronix", author: "Firefly Electronix") {
        capability "Button"
        capability "Switch"
	}


	simulator {
	}

	tiles(scale: 2) {

    valueTile("power", "device.power", decoration: "flat", width: 6, height: 4) {
            state "power", label:'${currentValue}v', backgroundColor:"#00a0dc"
        }

    main "power"
    details "power"

	}
}

// parse events into attributes
def parse(String description) {
	def msg = parseLanMessage(description)
  	def bodyString = msg.body
    def slurper = new groovy.json.JsonSlurper()
    def results = slurper.parseText(bodyString)
    log.debug("Battery: ${results.batt}")
    createEvent(name: "power", value: results.batt)
    createEvent(name: "button", value: "pushed", data: [buttonNumber: "1"], displayed: true, isStateChange: true)
    createEvent(name: "switch", value: "on", displayed: false, isStateChange: true)
    }
