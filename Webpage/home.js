class HomeCanvas extends p5{
    dim = [900, 580];
    colors = ['#3375FF', '#3396FF'];
    parent;
    sc;
    constructor(){
        super(sketch, window.document.getElementById("HomeCanvas"));
        this.setup = this.m_setup;
        this.draw = this.m_draw;
        this.mouseClicked = this.m_mouseclick;
        this.engine = this;
        this.parent = [];
        this.AddBox = function(pos, color){
            return{
                draw(engine){
                    engine.translate(pos.x, pos.y, pos.z);
                    engine.fill(color);
                    engine.box(150,50,300);
                }
            }
        }
        
        this.Boxes = [this.AddBox({x: 0, y: 0, z: 0}, this.colors[0]), this.AddBox({x: 0,y: -55, z: 0}, this.colors[1])]
        
        this.BoxStack = {
            size: 1,
            pos: {x: 0, y: 0, z: 0},
            rot: {theta: 0, axis: [0, 0, 0]},
            draw(engine){
                engine.push();
                engine.normalMaterial();
                // engine.rotate(this.rot.theta, this.axis);
                engine.translate(this.pos.x, this.pos.y, this.pos.z);
                engine.scale(this.size);
                engine.stroke(51);
                engine.Boxes.forEach(element => {
                    element.draw(engine);
                });
                engine.pop();
            }
        }

        this.info1 = new InfoBox({x: -15, y: -12}, 200);
        this.info1 = new InfoBox({x: -15, y: - 12 - 55}, 200);

        this.Bubbles = [new bubble({x: 150, y: 140}, 100), new bubble({x: 150, y: 0}, 100)];
        this.Bubbles.forEach(element => {
            element.appendix = true;
        });
        this.parent.push(new Map(this));
        // this.mape = new Map(this);

        this.transf = math.identity(3);
        this.visible = true;
        this.sc = 1;
        this.onPreload();
    }
    onPreload(){
        this.font = this.loadFont(
            "assets/Staatliches-Regular.ttf"
        );
        
        this.parent.forEach(element => {
            element.onPreload();
        });
    }
    m_setup(){
        this.createCanvas(3*this.displayWidth/5, 4*this.displayHeight/6, this.WEBGL);
        this.textFont(this.font);

    }

    m_draw(){
        this.background('rgb(255, 247, 235)');
        // this.orbitControl();
        
        
        
        this.push();
        this.perspective(Math.PI / 3.0, this.width / this.height, 1, 500);
        this.camera(10, 0, 350, 0, -55, 0, 0, -1, 0);

        this.BoxStack.draw(this);
        
        this.translate(0,0,300);
        this.scale(0.1);
        this.rotate(this.PI, [0,0,1]);
        this.Bubbles.forEach(element =>{
            element.draw(this.engine);
            // this.translate(0,-55,0);
        });
        this.pop();

        this.push();

        this.parent.forEach(element =>{
            element.draw(engine);
        });

        // this.mape.draw();
        this.pop();
    }

    m_mouseclick(){
        if(!this.visible) return;
        // this.detect_hitbox([this.mouseX,this.mouseY]);
    }
    detect_hitbox(point){
        if(this.visible){
            if(point[0] >= 0 && point[0] <= (this.dim[0]*this.sc)){
                if(point[1] >= 0 && point[1] <= (this.dim[1]*this.sc)){
                    this.default_dHit(point,this.transf);
                    return true;
                }
            }
        }
        return false;
    }
    onClick(){
        console.log("Click Home");
    }
    default_dHit(point,transform){
        this.onClick();
        this.parent.forEach(element => {
            element.detect_hitbox(point,this.transf);
        });
    }
}

var HOME_CANV = new HomeCanvas();