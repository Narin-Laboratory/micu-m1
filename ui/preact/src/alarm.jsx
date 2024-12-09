import { useEffect, useState } from 'react';

const alarmMessages = {
    111: "The microgreens are ready to be harvested."
};

const AlarmCard = ({ alarm }) => {
    const [visible, setVisible] = useState(true);

    useEffect(() => {
        setVisible(true);
        const timer = setTimeout(() => {
            setVisible(false);
        }, 30000);

        return () => clearTimeout(timer);
    }, [alarm.code]);

    if (!visible) return null;

    const alarmTime = new Date().toLocaleTimeString();

    return (
        <div className="alarm-card">
            <dialog open={visible && alarm.code != 0}>
                <article>
                    <header>
                    <button aria-label="Close" rel="prev" onClick={() => setVisible(false)}></button>
                    <p>
                        <strong>🚨 Alarm Code {alarm.code}</strong>
                    </p>
                    </header>
                    <p>
                        {alarmMessages[alarm.code] || "Unknown alarm code"}
                    </p>
                    <footer><small>Alarm time: {alarm.time}</small></footer>
                </article>
            </dialog>
        </div>
    );
};

export default AlarmCard;