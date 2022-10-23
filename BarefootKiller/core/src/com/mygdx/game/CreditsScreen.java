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

public class CreditsScreen implements Screen{

    private MainGame game;

    private Table table;
    private Skin skin;
    private Label author1;
    private Label author2;

    private Stage stage;

    public CreditsScreen(MainGame game){
        this.game = game;

        stage = new Stage();

        skin = new Skin(Gdx.files.internal("pixthulhu-ui.json"));

        table = new Table();
        table.setFillParent(true);

        Label.LabelStyle textStyle = new Label.LabelStyle();
        textStyle.font = skin.getFont("font");

        author1 = new Label("Adam Korta", textStyle);
        author2 = new Label("Filip Szczepanski", textStyle);
        author1.setFontScale(5);
        author2.setFontScale(5);
        table.add(author1);
        table.row();
        table.add(author2);
        table.row();

    }

    private TextButton addButton(String name){
        TextButton button = new TextButton(name, skin);
        table.add(button);
        table.row();

        return button;
    }

    @Override
    public void show() {

        stage.addActor(table);

        addButton("Back").addListener(new ClickListener(){
            @Override
            public void clicked(InputEvent event, float x, float y) {
                game.setScreen(new MainMenuScreen(game));
            }
        });

        Gdx.input.setInputProcessor(stage);

    }

    @Override
    public void render(float delta) {

        ScreenUtils.clear(0, 0.0f, 0.0f,1);

        stage.act();
        stage.draw();
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
