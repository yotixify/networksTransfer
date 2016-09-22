if(!packetBuffer.empty()){
				Packet initialPac = packetBuffer.top();
				int topLocation = initialPac.getLocation();
				while(!packetBuffer.empty() && topLocation == nextToWrite){
					Packet laterPac = packetBuffer.top();
					if(laterPac.getLocation() == nextToWrite){
						//write out the packet, increment next to write, and pop off the packet
						outFile.append(laterPac.getData(), laterPac.getSize());
						nextToWrite++;
						packetBuffer.pop();
					}
				}
			}