package com.mygdx.game;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Screen;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.scenes.scene2d.InputEvent;
import com.badlogic.gdx.scenes.scene2d.Stage;
import com.badlogic.gdx.scenes.scene2d.ui.Skin;
import com.badlogic.gdx.scenes.scene2d.ui.Table;
import com.badlogic.gdx.scenes.scene2d.ui.TextButton;
import com.badlogic.gdx.scenes.scene2d.utils.ClickListener;
import com.badlogic.gdx.utils.ScreenUtils;
import com.badlogic.gdx.scenes.scene2d.ui.Label;


public class GameOverScreen implements Screen {

    final MainGame game;

    OrthographicCamera camera;
    private Table table;
    private Skin skin;



    Label score;
    private Stage stage;

    public GameOverScreen(MainGame game, Label score) {
        this.game = game;

        camera = new OrthographicCamera(Gdx.graphics.getWidth(), Gdx.graphics.getHeight());

        this.score = score;
        this.score.setBounds(Gdx.graphics.getWidth() / 2 - 125, Gdx.graphics.getHeight() / 2 - 125, 250, 250);
        stage = new Stage();

        skin = new Skin(Gdx.files.internal("pixthulhu-ui.json"));

        table = new Table();
        table.setFillParent(true);

        table.add(score);
        table.row();


    }

    @Override
    public void show() {

        addButton("Restart").addListener(new ClickListener(){
            @Override
            public void clicked(InputEvent event, float x, float y) {
                game.setScreen(new GameScreen(game));
            }
        });

        addButton("Quit").addListener(new ClickListener(){
            @Override
            public void clicked(InputEvent event, float x, float y) {
                Gdx.app.exit();
            }
        });

        stage.addActor(table);

        Gdx.input.setInputProcessor(stage);

    }

    private TextButton addButton(String name){
        TextButton button = new TextButton(name, skin);
        table.add(button);
        table.row();

        return button;
    }



    @Override
    public void render(float delta) {

        ScreenUtils.clear(0, 0.0f, 0.0f,1);

        stage.draw();
        stage.act();
    }

    @Override
    public void resize(int width, int height) {

    }


    @Override
    public void pause() {

    }

    @Override
    public void resume() {

    }

    @Override
    public void hide() {

    }

    @Override
    public void dispose() {

    }
}
