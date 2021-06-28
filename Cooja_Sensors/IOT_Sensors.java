
/**
 * @{
 * \file
 *         Module to simulate Temperature, Humidity, AirFlow, Battery sensors in cooja
 *              Simulate values as ADC INPUTS
 *              Random Noise can be applied
 * 
 * \author
 *         Lucyanno Frota
 * 
 * Module based on Dave Hirsch's module for simulating Temperature
 */

package org.contikios.cooja.mspmote;


import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.GridLayout;
import java.util.Collection;
import java.awt.BorderLayout;
import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import java.util.Random;

import javax.swing.JSlider;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;

import java.util.logging.*;
import org.jdom.Element;

import org.contikios.cooja.ClassDescription;
import org.contikios.cooja.Mote;
import org.contikios.cooja.MoteInterface;
import org.contikios.cooja.MoteInterfaceHandler;
import org.contikios.cooja.interfaces.Position;
import org.contikios.cooja.Simulation;
import org.contikios.cooja.interfaces.Button;
import org.contikios.cooja.mspmote.MspMoteTimeEvent;
import org.contikios.cooja.mspmote.Z1Mote;
import se.sics.mspsim.core.ADC12;
import se.sics.mspsim.core.ADCInput;
import se.sics.mspsim.core.IOUnit;

@ClassDescription("IOT sensors")
public class IOT_Sensors extends MoteInterface {

    private static final int TEMPERATURE_MEM = 10;
    private static final int TEMPERATURE_id = 0;
    private static final int TEMPERATURE_DEFAULT_VALUE = 0;
    private static final int TEMPERATURE_MIN = -50;
    private static final int TEMPERATURE_MAX = 100;
    private static final boolean TEMPERATURE_ENABLE_NOISE = false;
    private static final int TEMPERATURE_NOISE = 51; // amplitude of random value
    private static final int TEMPERATURE_DIV = 1000;


    private static final int HUMIDITY_MEM = 12;
    private static final int HUMIDITY_id = 1;
    private static final int HUMIDITY_DEFAULT_VALUE = 40;
    private static final int HUMIDITY_MIN = 0;
    private static final int HUMIDITY_MAX = 100;
    private static final boolean HUMIDITY_ENABLE_NOISE = false;
    private static final int HUMIDITY_NOISE = 41; // amplitude of random value


    private static final int AIRFLOW_MEM = 13;
    private static final int AIRFLOW_id = 2;
    private static final int AIRFLOW_DEFAULT_VALUE = 20;
    private static final int AIRFLOW_MIN = 0;
    private static final int AIRFLOW_MAX = 150;
    private static final boolean AIRFLOW_ENABLE_NOISE = false;
    private static final int AIRFLOW_NOISE = 41; // amplitude of random value


    private static final int BATTERY_MEM = 11;
    private static final int BATTERY_id = 3;
    private static final int BATTERY_DEFAULT_VALUE = 60;
    private static final int BATTERY_MIN = 0;
    private static final int BATTERY_MAX = 100;
    private static final boolean BATTERY_ENABLE_NOISE = false;
    private static final int BATTERY_NOISE = 51; // amplitude of random value
    private static final int BATTERY_DIV = 1000;


    private static int[] lastValues = {TEMPERATURE_DEFAULT_VALUE, HUMIDITY_DEFAULT_VALUE, AIRFLOW_DEFAULT_VALUE, BATTERY_DEFAULT_VALUE};
    private static JLabel[] Indicators = new JLabel[4];
    private IOUnit adc = null;

    private Z1Mote z1Mote = null;

    private Random rand = new Random();

    protected class ADCsliderVal implements ADCInput {
        private JSlider myslider;
        private final int idx;

        public ADCsliderVal(JSlider slider,int id){
            myslider = slider;
            idx = id;
        }

        public int nextData(){
            if (myslider == null) {
                return -1;
            } else {
                if (myslider.getValue() != lastValues[idx]) {
                    lastValues[idx] = myslider.getValue();
                }
                // For academic purpose all values of (Temp, Humi, AirF) will be interpreted as uint16_t and will be cast to int16_t do to library limitations. In the sensor the data needs to be casted to uint16_t to make any sense.
                switch(idx){
                    case TEMPERATURE_id:
                        if(TEMPERATURE_ENABLE_NOISE) return (int) ((myslider.getValue() + (((rand.nextInt(2)==1) ? -1 : 1)*rand.nextInt(TEMPERATURE_NOISE)/TEMPERATURE_DIV))*436.9 + 21845);
                        else return (int) ((myslider.getValue())*436.9 + 21845);
                    case HUMIDITY_id:
                        if(HUMIDITY_ENABLE_NOISE) return (int) (myslider.getValue()*655.35 + ((rand.nextInt(2)==1) ? -1 : 1)*rand.nextInt(HUMIDITY_NOISE));
                        else return (int) (myslider.getValue()*655.35);
                    case AIRFLOW_id:
                        if(AIRFLOW_ENABLE_NOISE) return (int) (myslider.getValue()*436.9 + ((rand.nextInt(2)==1) ? -1 : 1)*rand.nextInt(AIRFLOW_NOISE));
                        else return (int) (myslider.getValue()*436.9);
                    case BATTERY_id:
                        if(BATTERY_ENABLE_NOISE) return (int) ((myslider.getValue()+((rand.nextInt(2)==1) ? -1 : 1)*rand.nextInt(BATTERY_NOISE)/TEMPERATURE_DIV)*(145.0/10.0)+2.75*1000);
                        else return (int) (myslider.getValue()*(145.0/10.0)+2.75*1000);
                }
                return -1;
            }  
        }
    }

    protected class Initi implements ADCInput {

        public Initi(){
            
        }

        public int nextData(){
            if (getZ1Mote() == null) {
                return 87;
            } else {
                return 0;
            }  
        }
    }

    public IOT_Sensors(Mote mote) {
        z1Mote = (Z1Mote) mote;
        adc = z1Mote.getCPU().getIOUnit("ADC12");
        if (adc instanceof ADC12) {
            ((ADC12) adc).setADCInput(TEMPERATURE_MEM, new Initi());
            ((ADC12) adc).setADCInput(HUMIDITY_MEM, new Initi());
            ((ADC12) adc).setADCInput(AIRFLOW_MEM, new Initi());
            ((ADC12) adc).setADCInput(BATTERY_MEM, new Initi());
        }

    }

    public Z1Mote getZ1Mote() {
        return z1Mote;
    }

    public class sensors_Slider extends JPanel{
        private JSlider slider = new JSlider();
        private JLabel label = new JLabel("");

        public sensors_Slider(int type) {
            slider.addChangeListener(new ChangeListener() {
                @Override
                public void stateChanged(ChangeEvent e) {
                    switch(type){
                        case TEMPERATURE_id:
                            label.setText("Temperature: "+String.valueOf(slider.getValue())+"ºc");
                        break;
                        case HUMIDITY_id:
                            label.setText("Humidity: "+String.valueOf(slider.getValue())+"%");
                        break;
                        case AIRFLOW_id:
                            label.setText("AirFlow: "+String.valueOf(slider.getValue())+"cm/s");
                        break;
                        case BATTERY_id:
                            label.setText("Battery: "+String.valueOf(slider.getValue())+"%");
                        break;
                    }
                }
            });
            switch(type){
                case 0:
                    slider.setMinimum(TEMPERATURE_MIN);
                    slider.setMaximum(TEMPERATURE_MAX);
                    slider.setValue(TEMPERATURE_DEFAULT_VALUE);
                break;
                case 1:
                    slider.setMinimum(HUMIDITY_MIN);
                    slider.setMaximum(HUMIDITY_MAX);
                    slider.setValue(HUMIDITY_DEFAULT_VALUE);
                break;
                case 2:
                    slider.setMinimum(AIRFLOW_MIN);
                    slider.setMaximum(AIRFLOW_MAX);
                    slider.setValue(AIRFLOW_DEFAULT_VALUE);
                break;
                case 3:
                    slider.setMinimum(BATTERY_MIN);
                    slider.setMaximum(BATTERY_MAX);
                    slider.setValue(BATTERY_DEFAULT_VALUE);
                break;
            }
    
            slider.setMajorTickSpacing(20);
            slider.setMinorTickSpacing(5);
            slider.setPaintTicks(true);
            slider.setPaintLabels(true);
    
            JPanel topPanel = new JPanel();
            topPanel.add(label);
    
            setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
            setLayout(new BorderLayout(5, 5));
            add(topPanel, BorderLayout.PAGE_START);
            add(slider);
        }

        JSlider getSlider(){
            return slider;
        }

    }

    public JPanel getInterfaceVisualizer() {
        JPanel panel = new JPanel(new GridLayout(4,1));
        sensors_Slider sADC0 = new sensors_Slider(0);
        sensors_Slider sADC1 = new sensors_Slider(1);
        sensors_Slider sADC2 = new sensors_Slider(2);
        sensors_Slider sADC3 = new sensors_Slider(3);
        panel.add(sADC0);
        panel.add(sADC1);
        panel.add(sADC2);
        panel.add(sADC3);
        if (adc != null) {
            ((ADC12) adc).setADCInput(TEMPERATURE_MEM, new ADCsliderVal(sADC0.getSlider(),0));
            ((ADC12) adc).setADCInput(HUMIDITY_MEM, new ADCsliderVal(sADC1.getSlider(),1));
            ((ADC12) adc).setADCInput(AIRFLOW_MEM, new ADCsliderVal(sADC2.getSlider(),2));
            ((ADC12) adc).setADCInput(BATTERY_MEM, new ADCsliderVal(sADC3.getSlider(),3));
        }
        return panel;
    }

    public void releaseInterfaceVisualizer(JPanel panel) {
    }

    public Collection<Element> getConfigXML() {
        return null;
    }

    public void setConfigXML(Collection<Element> configXML, boolean visAvailable) {
    }
}

