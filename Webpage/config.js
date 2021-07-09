class ConfigCanvas extends p5{
    map;
    constructor(){
        super(sketch, window.document.getElementById("ConfigCanvas"));
        this.setup = this.m_setup;
        this.draw = this.m_draw;
        this.map = new Map(this);
    }

    m_setup(){
        this.createCanvas(960, 600);
        this.background(0);
    }

    m_draw(){
        this.map.draw();
        // if(this.mouseIsPressed) {
        //     this.fill(0);
        // }else{
        //     this.fill(255);
        // }
        // this.ellipse(this.mouseX, this.mouseY, 80, 80);
    }
}

new ConfigCanvas();