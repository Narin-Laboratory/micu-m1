import { h } from 'preact';
import { useState } from 'preact/hooks';
import { useAppState } from './AppStateContext';
import CryptoJS from 'crypto-js';

const LoginPopUp = () => {
  const { status, setStatus, ws, sendWsMessage, authState, salt, wsStatus, cfg } = useAppState();
  const [htP, setHtP] = useState('');

  const handleChange = (event) => {
    setHtP(event.target.value);
  };

  const handleSubmit = (event) => {
    event.preventDefault();

    // Convert htP and salt into CryptoJS word arrays for consistent byte interpretation
    const key = CryptoJS.enc.Utf8.parse(htP);
    //const key = CryptoJS.enc.Utf8.parse("milikpetani");
    const saltUtf8 = CryptoJS.enc.Utf8.parse(salt.salt);

    // Compute the HMAC SHA-256
    const hmac = CryptoJS.HmacSHA256(saltUtf8, key); // Note: reversed order (salt as message, key as key)
    const hmacHex = hmac.toString(CryptoJS.enc.Hex).toLowerCase();
    
    // Send to server
    sendWsMessage({ auth: {hash: hmacHex, salt: salt.salt }});
  };

  return (
    <dialog open={!authState && wsStatus && cfg.fInit}>
      <article>
        <header>
          <hgroup>
            <h2>MICU Smart Grow</h2>
            <p>Microgreen cultivator and indoor smart seedling</p>
          </hgroup>
        </header>
        <form onSubmit={handleSubmit}>
          <fieldset role="group"> 
            <input name="htP" onChange={handleChange} type="password" placeholder="Agent secret" autoComplete="password" />
            <input type="submit" value="Login" />
          </fieldset>
          <small>{status.msg != "" ? status.msg : "Enter your agent secret to access this agent."}</small>
        </form>
        <footer>
          <small><i>{salt.name} agent at {salt.group}</i></small>
        </footer>
      </article>
    </dialog>
  );
};

export default LoginPopUp;
