class HomeCanvas extends p5{
    constructor(){
        super(sketch, window.document.getElementById("HomeCanvas"));
        this.setup = this.m_setup;
        this.draw = this.m_draw;
    }

    m_setup(){
        this.createCanvas(900, 580,this.WEBGL);
        this.perspective(Math.PI / 3.0, this.width / this.height, 0.1, 1000);
        this.camera(150, -120, 300, 0, 0, 0, 0, 1, 0);
    }

    m_draw(){
        this.background(200);
        // this.orbitControl();
        this.normalMaterial();

        this.translate(0, 50, -50);
        this.smooth();
        this.push();
        this.stroke(51);
        this.strokeWeight(2);
        this.fill('#3396FF');
        this.box(150,50,150);

        this.translate(0, -55, 0);
        this.fill('#33C3FF');
        this.box(150,50,150);
        this.translate(0, -55, 0);
        this.fill('#3375FF');
        this.box(150,50,150);
        this.pop();
    }
}

new HomeCanvas();