import { render } from 'preact';
import { useState, useEffect } from 'preact/hooks';

import './style.css';
import SettingsIcon from './assets/settings.svg';
import WiFiIcon from './assets/wifi.svg';
import databaseIcon from './assets/database.svg';
import clockIcon from './assets/clock.svg';
import watchIcon from './assets/watch.svg';
import heartIcon from './assets/heart.svg';
import { AppStateProvider, useAppState } from './AppStateContext';
import SetupForm from './setupForm';
import LoginPopUp from './loginPopUp';
import AlarmCard from './alarm';

function App() {
	return (
	  <AppStateProvider>
		<MainApp />
	  </AppStateProvider>
	);
  }
  
  function MainApp() {
	const { cfg, ws, sendWsMessage, authState, showSetupForm, setShowSetupForm, wsStatus, finishedSetup, setFinishedSetup } = useAppState();
	const [latestCfg, setLatestCfg] = useState(cfg); // State to hold latest cfg
	const [alarm, setAlarm] = useState({code: 0, time: ''});
	const [sysInfo, setSysInfo] = useState({uptime: 0, heap: 0, datetime: 0, rssi: 0});

	const [micuM1State, setMicuM1State] = useState({
		growLightState: false,
		pumpState: false,
		mode: 0,
		sowingDatetime: '',
		selectedProfile: ''
	});

	const [profiles, setprofiles] = useState([{
		"id": 0,
		"name": "",
		"incubationTS": 0
	}]);

	const [micuM1Stream, setMicuM1Stream] = useState({
		"remainingTimeToHarvest": 0
	});

	const handleSowingDatetimeChange = (event) => {
		const gmtOffset = cfg.gmtOff || 0;
		const adjustedDatetime = new Date(new Date(event.target.value).getTime() + (gmtOffset*1000)).toISOString().slice(0, 19).replace('T', ' ');
		micuM1State.sowingDatetime = adjustedDatetime;
		setMicuM1State(micuM1State);
		const payload = {
			sowingDatetime: adjustedDatetime
		};
		sendWsMessage(payload);
	};

	const handleProfileChange = (event) => {
		const newProfileId = event.target.value;
		micuM1State.selectedProfile = newProfileId;
		setMicuM1State(micuM1State);
		// Send the command via WebSocket
		const payload = {
			profile: newProfileId
		};
		sendWsMessage(payload);
	};

	const handleToggleSwitchChange = (event) => {
		const newState = event.target.checked;
		if(event.target.name === 'growLightState'){
			micuM1State.growLightState = newState;
			setMicuM1State(micuM1State);
			// Send the command via WebSocket
			const payload = {
				brightness: newState ? 100 : 0
			};
			sendWsMessage(payload);	
		}
		else if(event.target.name === 'pumpState'){
			micuM1State.pumpState = newState;
			setMicuM1State(micuM1State);
			// Send the command via WebSocket
			const payload = {
				power: newState ? 100 : 0
			};
			sendWsMessage(payload);	
		}
		else if(event.target.name === 'mode'){
			micuM1State.mode = newState;
			setMicuM1State(micuM1State);
			// Send the command via WebSocket
			const payload = {
				mode: newState ? 1 : 0
			};
			sendWsMessage(payload);	
		}
	};

  
	useEffect(() => {
		// Function to update latestCfg whenever cfg changes
		const updateCfg = () => {
			setLatestCfg(cfg); 
		};

		// Subscribe to WebSocket messages
		if (ws.current) {
			ws.current.addEventListener('message', (event) => {
				const data = JSON.parse(event.data);
				//console.log(data);
				if (data.cfg) {
					updateCfg(); // Update latestCfg when new config is received
				} else if (data.micuM1State) {
					setMicuM1State(data.micuM1State);
				} else if (data.alarm && data.alarm.code !== 0) {
					setAlarm(data.alarm);
				} else if (data.setFinishedSetup) {
					setFinishedSetup(data.setFinishedSetup.fInit);
				} else if (data.sysInfo) {
					setSysInfo(data.sysInfo);
				} else if (data.profiles) {
					setprofiles(data.profiles);
				} else if (data.micuM1Stream) {
					setMicuM1Stream(data.micuM1Stream);
				}
				
			});
		}

		// Cleanup: Remove the event listener when component unmounts
		return () => {
			if (ws.current) {
				ws.current.removeEventListener('message', updateCfg);
			}
		};
	}, [cfg, ws]); // Run effect whenever cfg or ws changes

	const handleShowSetupForm = () => {
		setShowSetupForm(!showSetupForm);
	};
	return (
	  <div>
		{ !wsStatus && (
		<article class="full-page-cover" data-theme="dark">
		  {/* Your cover content here */}
		  <h1>Websocket Connect Failed</h1>
		  <p>Unable to connect to MICU device. Please make sure that you are in the same WiFi network with the device.</p>
		  <div>😵</div>
		</article>
	  )}
		<header>
			<article>
				<nav>
					<ul>
						<li><a onClick={handleShowSetupForm} class="secondary" aria-label="Menu" data-discover="true" href="#">
							<img src={SettingsIcon} alt="Settings" />
						</a></li></ul>
					<ul>
						<li><strong>MICU {cfg.model}</strong></li>
					</ul>
				</nav>
			</article>
			<div id="indicator-bar">
				<div className={"parent-3c"}>
					<div className={"indicater-item"}>
						<img src={WiFiIcon} alt="WiFi" /> {sysInfo.rssi}%
					</div>
					<div className={"indicater-item"}>
						<img src={databaseIcon} alt="heap" /> {(sysInfo.heap/1024).toFixed(0)}Kb
					</div>
					<div className={"indicater-item"}>
						<img src={watchIcon} alt="uptime" /> {(sysInfo.uptime/1000).toFixed(0)}sec
					</div>
				</div>
				<div className={"indicater-item"}>
					<img src={clockIcon} alt="datetime" /> {sysInfo.datetime}
				</div>
			</div>
		</header>
		<main class="container">
		<dialog open={finishedSetup}>
			<article>
				<header>
				<p>
					<strong>Device Setup Completed!</strong>
				</p>
				</header>
				<p>
				Now you can connect to WiFi <strong>{latestCfg.wssid}</strong> and access the agent built-in web interface via <br/><br/>
				<a href={`http://${latestCfg.hname}.local`}><strong>http://{latestCfg.hname}.local</strong></a><br/><br/><br/>
				Thankyou and happy farming!
				</p>
			</article>
		</dialog>
			<LoginPopUp />
			{!cfg.fInit ? (
				<section>
					<SetupForm />
				</section>
			) : (
				<div>
					{authState && (
					<section>
						<article>
							<label>
								<input name="mode" type="checkbox" role="switch" checked={micuM1State.mode == 0 ? false : true} onChange={handleToggleSwitchChange} />
								Mode: {micuM1State.mode == 0 ? 'Manual' : 'Auto'}
							</label>
							<label>
								Profile:
								<select
									name="selectedProfile"
									value={micuM1State.selectedProfile}
									onChange={handleProfileChange}>
									{profiles.map((profile) => (
										<option key={profile.id} value={profile.id}>
											{profile.name}
										</option>
									))}
								</select>
								<label>
									<input type="datetime-local" name="sowingDatetime" aria-label="Datetime" value={micuM1State.sowingDatetime ? new Date((new Date(micuM1State.sowingDatetime).getTime() + (cfg.gmtOff*1000) )).toISOString().slice(0, 16) : ''} onChange={handleSowingDatetimeChange}></input>
									<small id="datetime-helper">
									Sowing datetime in your local timezone.
									</small>
								</label>
								{micuM1Stream.remainingTimeToHarvest > 0 && (
									<div>
										Remaining Time to Harvest: {Math.floor(micuM1Stream.remainingTimeToHarvest / 86400)} days {new Date(micuM1Stream.remainingTimeToHarvest * 1000).toISOString().substr(11, 8)}
									</div>
								)}
							</label>
							<AlarmCard alarm={alarm}></AlarmCard>
						</article>
						<article>
							<div class="parent-2c">
								<label>
									<input name="growLightState" type="checkbox" role="switch" checked={micuM1State.growLightState} onChange={handleToggleSwitchChange} disabled={ micuM1State.mode == 0 ? false : true}/>
									Lampu: {micuM1State.growLightState ? 'ON' : 'OFF'}
								</label>
								<label>
									<input name="pumpState" type="checkbox" role="switch" checked={micuM1State.pumpState} onChange={handleToggleSwitchChange} disabled={ micuM1State.mode == 0 ? false : true}/>
									Pompa: {micuM1State.pumpState ? 'ON' : 'OFF'}
								</label>
							</div>
						</article>
						<dialog open={showSetupForm && !finishedSetup}>
							<article>
								<SetupForm></SetupForm>
							</article>
						</dialog>
					</section>
					)}
				</div>
			)}
		</main>
		<footer>
		  <section class="text-center mt-10">
			<hr />
			<div id="copyleft">
				<div class="copyleft-item">
					<a href="https://udawa.or.id" target="_blank"><img src={heartIcon} alt="heartIcon" /> PSTI UNDIKNAS <img src={heartIcon} alt="heartIcon" /></a>
				</div>
			</div>
		  </section>
		</footer>
	  </div>
	);
  }
  
  render(<App />, document.getElementById('app'));